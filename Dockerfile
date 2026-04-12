# syntax=docker/dockerfile:1
# Use Ubuntu 24.04 as base
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake ninja-build pkg-config git curl unzip \
    gcc-13 \
    g++-13 \
    python3 \
    libgl1-mesa-dev libglu1-mesa-dev libglew-dev \
    libx11-dev libxi-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libxxf86vm-dev \
    libwayland-dev libxkbcommon-dev wayland-protocols \
    libglm-dev \
    libfreetype6-dev \
    libbz2-dev zlib1g-dev \
    liblua5.3-dev \
    libboost-all-dev \
    qtbase5-dev qtbase5-dev-tools qtbase5-private-dev \
    qttools5-dev qttools5-dev-tools \
    qtdeclarative5-dev qtquickcontrols2-5-dev \
    qtmultimedia5-dev \
    libqt5svg5-dev libqt5x11extras5-dev \
    libqt5opengl5-dev \
    && rm -rf /var/lib/apt/lists/*

# Fix Qt5 headers: Ubuntu installs them without .h extension, source uses <QtWidgets/QFoo.h>
RUN find /usr/include/x86_64-linux-gnu/qt5 -maxdepth 2 -name 'Q*' ! -name '*.*' \
    | while read f; do ln -sf "$f" "${f}.h"; done

WORKDIR /tmp/noggit-red

RUN git clone https://github.com/ladislav-zezula/StormLib.git StormLib && \
    cmake -S StormLib -B StormLib/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON && \
    cmake --build StormLib/build -j$(nproc) && \
    cmake --install StormLib/build && \
    mkdir /tmp/bz2obj && cd /tmp/bz2obj && \
    ar x /usr/lib/x86_64-linux-gnu/libbz2.a && \
    ar qf /usr/local/lib/libstorm.a /tmp/bz2obj/*.o && \
    rm -rf /tmp/bz2obj

RUN git clone https://github.com/ladislav-zezula/CascLib.git CascLib && \
    cmake -S CascLib -B CascLib/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON && \
    cmake --build CascLib/build -j$(nproc) && \
    cmake --install CascLib/build

RUN git clone https://github.com/Auburn/FastNoise2.git FastNoise2 && \
    cmake -S FastNoise2 -B FastNoise2/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DFASTNOISE2_NOISETOOL=OFF && \
    cmake --build FastNoise2/build -j$(nproc) && \
    cmake --install FastNoise2/build

COPY . .

RUN git submodule update --init --recursive 2>/dev/null || true

RUN python3 << 'EOF'
import os, glob, re

explicit_fixes = [
    ('src/noggit/SceneObject.hpp',
     '#include <noggit/MapHeaders.h>', '#pragma once'),
    ('src/external/blizzard-database-library/include/extensions/VectorExtensions.h',
     '#include <algorithm>', None),
]

bulk_fixes = [
    ('ExpanderWidget', '#include <noggit/ui/tools/UiCommon/expanderwidget.h>', 'expanderwidget'),
    ('LogError',       '#include <noggit/Log.h>',                              None),
    ('LogDebug',       '#include <noggit/Log.h>',                              None),
]

# Lowercase Qt includes that should be capitalised
qt_case_fixes = [
    ('<qtemporaryfile>',  '<QTemporaryFile>'),
    ('<qtemporarydir>',   '<QTemporaryDir>'),
    ('<qfile>',           '<QFile>'),
    ('<qdir>',            '<QDir>'),
]

def read(path):
    return open(path, encoding='latin-1').read()

def write(path, content):
    open(path, 'w', encoding='latin-1').write(content)

def insert_include(content, inc):
    lines = content.split('\n')
    insert_at = next((i + 1 for i, l in enumerate(lines)
                      if l.startswith('#pragma once') or l.startswith('#include')), 0)
    lines.insert(insert_at, inc)
    return '\n'.join(lines)

for path, inc, anchor in explicit_fixes:
    if not os.path.exists(path):
        print(f'SKIP (not found): {path}'); continue
    content = read(path)
    if inc in content:
        print(f'SKIP (already present): {path}'); continue
    if anchor and anchor in content:
        content = content.replace(anchor, anchor + '\n' + inc, 1)
    else:
        content = insert_include(content, inc)
    write(path, content)
    print(f'PATCHED: {path}')

for symbol, inc, exclude in bulk_fixes:
    for path in glob.glob('src/**/*.cpp', recursive=True) + \
                glob.glob('src/**/*.hpp', recursive=True) + \
                glob.glob('src/**/*.h',   recursive=True):
        if not os.path.exists(path): continue
        if exclude and exclude.lower() in path.lower(): continue
        content = read(path)
        if symbol in content and inc not in content:
            write(path, insert_include(content, inc))
            print(f'PATCHED: {path}')

all_src = (glob.glob('src/**/*.cpp', recursive=True) +
           glob.glob('src/**/*.hpp', recursive=True) +
           glob.glob('src/**/*.h',   recursive=True))

for path in all_src:
    if not os.path.exists(path): continue
    content = read(path)
    new_content = content

    # Pass 1: fix "Foo\\Bar\\" style path segments in string literals
    while True:
        replaced = re.sub(r'(["\'\`])([A-Za-z][^\'"]*?)\\\\([^\'"]*?\1)',
                         lambda m: m.group(1) + m.group(2) + '/' + m.group(3), new_content)
        if replaced == new_content:
            break
        new_content = replaced

    # Pass 2: fix standalone path separator strings like "\\" -> "/"
    new_content = re.sub(r'"\\\\+"', '"/"', new_content)

    for old, new in qt_case_fixes:
        new_content = new_content.replace(old, new)

    if new_content != content:
        write(path, new_content)
        print(f'PATCHED: {path}')

# Targeted source fixes for Linux compatibility.
# Note: old strings must match the post-path-fix state of the files,
# since source_patches runs after the path regex loop above.
source_patches = [
    # Fix: map_horizon - initialise _qt_minimap before the WDL-not-found early return,
    # so that setPixel calls on new maps don't write to a null 0x0 image.
    (
        'src/noggit/map_horizon.cpp',
        'map_horizon::map_horizon(const std::string& basename, const MapIndex * const index)\n{\n  std::stringstream filename;',
        'map_horizon::map_horizon(const std::string& basename, const MapIndex * const index)\n{\n  _qt_minimap = QImage(16 * 64, 16 * 64, QImage::Format_ARGB32);\n  _qt_minimap.fill(Qt::transparent);\n\n  std::stringstream filename;',
    ),
    # Fix: minimap_widget - colour newly added tiles green instead of grey
    # so tile selection is visible in the MapCreationWizard.
    (
        'src/noggit/ui/minimap_widget.cpp',
        'painter.setPen (QColor::fromRgbF (0.8f, 0.8f, 0.8f, 0.4f));',
        'painter.setPen (QColor::fromRgbF (0.0f, 0.8f, 0.0f, 0.8f));',
    ),
    # Fix: IsEditableWorld - normalise path to lowercase before exists() and ClientFile
    # so it matches the on-disk structure (world/maps/) on case-sensitive Linux filesystems.
    # The directory is created lowercase by saveCurrentEntry via normalizeFilenameInternal.
    (
        'src/noggit/World.cpp',
        '  ssfilename << "World/Maps/" << lMapName << "/" << lMapName << ".wdt";\n\n  if (!Noggit::Application::NoggitApplication::instance()->clientData()->exists(ssfilename.str()))\n  {\n    Log << "World " << record.RecordId << ": " << lMapName << " has no WDT file!" << std::endl;\n    return false;\n  }\n\n  BlizzardArchive::ClientFile mf(ssfilename.str(), Noggit::Application::NoggitApplication::instance()->clientData());',
        '  ssfilename << "World/Maps/" << lMapName << "/" << lMapName << ".wdt";\n  auto wdt_path = BlizzardArchive::ClientData::normalizeFilenameInternal(ssfilename.str());\n\n  if (!Noggit::Application::NoggitApplication::instance()->clientData()->exists(wdt_path))\n  {\n    Log << "World " << record.RecordId << ": " << lMapName << " has no WDT file!" << std::endl;\n    return false;\n  }\n\n  BlizzardArchive::ClientFile mf(wdt_path, Noggit::Application::NoggitApplication::instance()->clientData());',
    ),
    # Fix: IsWMOWorld - same normalisation fix as IsEditableWorld.
    (
        'src/noggit/World.cpp',
        '    ssfilename << "World/Maps/" << lMapName << "/" << lMapName << ".wdt";\n\n    BlizzardArchive::ClientFile mf(ssfilename.str(), Noggit::Application::NoggitApplication::instance()->clientData());',
        '    ssfilename << "World/Maps/" << lMapName << "/" << lMapName << ".wdt";\n    auto wdt_path = BlizzardArchive::ClientData::normalizeFilenameInternal(ssfilename.str());\n\n    BlizzardArchive::ClientFile mf(wdt_path, Noggit::Application::NoggitApplication::instance()->clientData());',
    ),
    # Fix: ClientData::getDiskPath - use normalizeFilenameInternal (lowercase + slash fix)
    # instead of normalizeFilenameUnix (slash fix only), so all on-disk project file paths
    # are consistently lowercase and match the directory structure created by the wizard.
    (
        'src/external/blizzard-archive-library/src/ClientData.cpp',
        '    return (fs::path(_local_path) / ClientData::normalizeFilenameUnix(file_key.filepath())).string();\n  }\n  else\n  {\n    // try deducing filepath from listfile\n    assert(file_key.hasFileDataID());\n    std::string filepath = _listfile.getPath(file_key.fileDataID());\n\n    if (!filepath.empty())\n    {\n      return (fs::path(_local_path) / ClientData::normalizeFilenameUnix(filepath)).string();',
        '    return (fs::path(_local_path) / ClientData::normalizeFilenameInternal(file_key.filepath())).string();\n  }\n  else\n  {\n    // try deducing filepath from listfile\n    assert(file_key.hasFileDataID());\n    std::string filepath = _listfile.getPath(file_key.fileDataID());\n\n    if (!filepath.empty())\n    {\n      return (fs::path(_local_path) / ClientData::normalizeFilenameInternal(filepath)).string();',
    ),
    # Fix: MapCreationWizard - remove the empty placeholder WDT file creation.
    # MapIndex::save() already writes the real WDT via ClientFile. The zero-byte
    # placeholder was being written to a differently-cased path, causing IsEditableWorld
    # to find an empty file and return false, filtering the map out of the list.
    (
        'src/noggit/ui/tools/MapCreationWizard/Ui/MapCreationWizard.cpp',
        '    auto filepath = project_path / BlizzardArchive::ClientData::normalizeFilenameInternal (filename.str());\n\n    QFile file(filepath.string().c_str());\n    file.open(QIODevice::WriteOnly);\n    file.close();',
        '    // WDT is written by mapIndex.save() below - no placeholder needed',
    ),
]

for filepath, old, new in source_patches:
    if not os.path.exists(filepath):
        print(f'SKIP (not found): {filepath}'); continue
    content = read(filepath)
    if old not in content:
        print(f'SKIP (already patched or not matched): {filepath}'); continue
    write(filepath, content.replace(old, new, 1))
    print(f'PATCHED: {filepath}')

EOF

RUN cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=gcc-13 \
        -DCMAKE_CXX_COMPILER=g++-13 \
        -DCMAKE_CXX_FLAGS="-Wno-error -Wno-deprecated -Wno-deprecated-enum-enum-conversion -Wno-unknown-warning-option" \
        -DNOGGIT_OPENGL_ERROR_CHECK=OFF \
        -DNOGGIT_ENABLE_TRACY_PROFILER=OFF \
        -DFASTNOISE2_NOISETOOL=OFF \
        -DCMAKE_PREFIX_PATH="/usr/local;/usr/lib/x86_64-linux-gnu/cmake/Qt5" \
        -DBoost_NO_BOOST_CMAKE=ON && \
    bash -c 'cmake --build build -j$(nproc) 2>&1 | tee /tmp/build.log; \
             ret=${PIPESTATUS[0]}; \
             grep -E " error:" /tmp/build.log | grep -v "warning:" | head -40; \
             exit $ret'
