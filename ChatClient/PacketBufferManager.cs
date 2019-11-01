using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace csharp_test_client
{
    class PacketBufferManager
    {
        int BufferSize = 0;
        int ReadPos = 0;
        int WritePos = 0;

        int HeaderSize = 0;
        int MaxPacketSize = 0;
        byte[] PacketData;
        byte[] PacketDataTemp;

        public bool Init(int size, int headerSize, int maxPacketSize)
        {
            if (size < (maxPacketSize * 2) || size < 1 || headerSize < 1 || maxPacketSize < 1)
            {
                return false;
            }

            BufferSize = size;
            PacketData = new byte[size];
            PacketDataTemp = new byte[size];
            HeaderSize = headerSize;
            MaxPacketSize = maxPacketSize;

            return true;
        }

        public bool Write(byte[] data, int pos, int size)
        {
            if (data == null || (data.Length < (pos + size)))
            {
                return false;
            }

            var remainBufferSize = BufferSize - WritePos;

            if (remainBufferSize < size)
            {
                return false;
            }

            Buffer.BlockCopy(data, pos, PacketData, WritePos, size);
            WritePos += size;

            if (NextFree() == false)
            {
                BufferRelocate();
            }
            return true;
        }

        public ArraySegment<byte> Read()
        {
            var enableReadSize = WritePos - ReadPos;

            if (enableReadSize < HeaderSize)
            {
                return new ArraySegment<byte>();
            }

            var packetDataSize = BitConverter.ToInt16(PacketData, ReadPos);
            if (enableReadSize < packetDataSize)
            {
                return new ArraySegment<byte>();
            }

            var completePacketData = new ArraySegment<byte>(PacketData, ReadPos, packetDataSize);
            ReadPos += packetDataSize;
            return completePacketData;
        }

        bool NextFree()
        {
            var enableWriteSize = BufferSize - WritePos;

            if (enableWriteSize < MaxPacketSize)
            {
                return false;
            }

            return true;
        }

        void BufferRelocate()
        {
            var enableReadSize = WritePos - ReadPos;

            Buffer.BlockCopy(PacketData, ReadPos, PacketDataTemp, 0, enableReadSize);
            Buffer.BlockCopy(PacketDataTemp, 0, PacketData, 0, enableReadSize);

            ReadPos = 0;
            WritePos = enableReadSize;
        }
    }
}
