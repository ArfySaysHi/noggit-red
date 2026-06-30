#include <stream/BitReader.h>

namespace BlizzardDatabaseLib {
    namespace Stream {

        BitReader::BitReader(std::unique_ptr<char[]>& dataStart, unsigned int dataLength) : _dataStart(dataStart), DataLength(dataLength), Position(0), Offset(0)
        {

        }

        std::string BitReader::ReadNullTermintingString()
        {
            std::stringstream stream;
            unsigned char byte;
            while ((byte = (unsigned char)ReadUint32(8)) != '\0')
                stream << byte;

            return stream.str();
        }

        unsigned int BitReader::ReadUint32(int numberOfBits)
        {
            auto index = Offset + (Position >> 3);
            auto p1 = (32 - numberOfBits - (Position & 7));
            auto p2 = (32 - numberOfBits);
            auto ptrSize = sizeof(unsigned int);

            unsigned int result;
            memcpy(&result, _dataStart.get() + index, ptrSize);

            result = result << p1 >> p2;

            Position += numberOfBits;
            return result;
        }

        unsigned long long BitReader::ReadUint64(int numberOfBits)
        {
            auto index = Offset + (Position >> 3);
            auto p1 = (64 - numberOfBits - (Position & 7));
            auto p2 = (64 - numberOfBits);
            auto ptrSize = sizeof(unsigned long long);

            unsigned long long result;
            memcpy(&result, _dataStart.get() + index, ptrSize);

            result = result << p1 >> p2;

            Position += numberOfBits;
            return result;
        }

        Structures::Int64 BitReader::ReadValue64(int numberOfBits)
        {
            auto value = Structures::Int64();
            value.ULongLong = ReadUint64(numberOfBits);
            return value;
        }

        Structures::Int64 BitReader::ReadSignedValue64(int numberOfBits)
        {
            auto value = Structures::Int64();
            value.ULongLong = ReadUint64(numberOfBits);
            auto signedShift = 1UL << (numberOfBits - 1);
            value.LongLong = (signedShift ^ value.ULongLong) - signedShift;
            return value;
        }
    }
}