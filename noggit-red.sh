#!/bin/bash
# Create the config dir with the correct capitalisation if it doesn't exist.
# Qt5 writes QSettings to ~/.config/Noggit/Noggit.conf (capital N).
mkdir -p "$HOME/.config/Noggit"
docker rm -f noggit 2>/dev/null || true

docker run -it --rm \
  --name noggit \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -e XDG_RUNTIME_DIR=/tmp \
  --device /dev/dri \
  --ipc=host \
  -v /home/$USER/Games/acwow/ChromieCraft_3.3.5a:/wow \
  -v /home/$USER/noggit-projects:/projects \
  -v /home/$USER/.config/Noggit:/root/.config/Noggit \
  noggit-red \
  /tmp/noggit-red/build/bin/noggit
