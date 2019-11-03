using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace csharp_test_client
{
    struct PacketData
    {
        public Int16 DataSize;
        public Int16 PacketID;
        public SByte Type;
        public byte[] BodyData;
    }

    public class PacketDump
    {
        public static string Bytes(byte[] byteArr)
        {
            StringBuilder sb = new StringBuilder("[");
            for (int i = 0; i < byteArr.Length; ++i)
            {
                sb.Append(byteArr[i] + " ");
            }
            sb.Append("]");
            return sb.ToString();
        }
    }
    

    public class ErrorNtfPacket
    {
        public ERROR_CODE Error;

        public bool FromBytes(byte[] bodyData)
        {
            Error = (ERROR_CODE)BitConverter.ToInt16(bodyData, 0);
            return true;
        }
    }
    

    public class LoginReqPacket
    {
        byte[] UserID = new byte[PacketDef.MAX_USER_ID_BYTE_LENGTH];
        byte[] UserPW = new byte[PacketDef.MAX_USER_PW_BYTE_LENGTH];

        public void SetValue(string userID, string userPW)
        {
            Encoding.UTF8.GetBytes(userID).CopyTo(UserID, 0);
            Encoding.UTF8.GetBytes(userPW).CopyTo(UserPW, 0);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(UserID);
            dataSource.AddRange(UserPW);
            return dataSource.ToArray();
        }
    }

    public class LoginResPacket
    {
        public UInt16 Result;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }


    public class RoomEnterReqPacket
    {
        int RoomNumber;
        public void SetValue(int roomNumber)
        {
            RoomNumber = roomNumber;
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(BitConverter.GetBytes(RoomNumber));
            return dataSource.ToArray();
        }
    }

    public class RoomEnterResPacket
    {
        public UInt16 Result;
        //public Int64 RoomUserUniqueId;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            //RoomUserUniqueId = BitConverter.ToInt64(bodyData, 2);
            return true;
        }
    }

    public class RoomUserListNtfPacket
    {
        public int UserCount = 0;
        public List<Int64> UserUniqueIdList = new List<Int64>();
        public List<string> UserIDList = new List<string>();

        public bool FromBytes(byte[] bodyData)
        {
            var readPos = 0;
            var userCount = (SByte)bodyData[readPos];
            ++readPos;

            for (int i = 0; i < userCount; ++i)
            {
                var uniqeudId = BitConverter.ToInt64(bodyData, readPos);
                readPos += 8;

                var idlen = (SByte)bodyData[readPos];
                ++readPos;

                var id = Encoding.UTF8.GetString(bodyData, readPos, idlen);
                readPos += idlen;

                UserUniqueIdList.Add(uniqeudId);
                UserIDList.Add(id);
            }

            UserCount = userCount;
            return true;
        }
    }

    public class RoomNewUserNtfPacket
    {
        public Int64 UserUniqueId;
        public string UserID;

        public bool FromBytes(byte[] bodyData)
        {
            var readPos = 0;

            UserUniqueId = BitConverter.ToInt64(bodyData, readPos);
            readPos += 8;

            var idlen = (SByte)bodyData[readPos];
            ++readPos;

            UserID = Encoding.UTF8.GetString(bodyData, readPos, idlen);
            readPos += idlen;

            return true;
        }
    }


    public class RoomChatReqPacket
    {
        byte[] Msg = new byte[PacketDef.MAX_CHAT_MSG_SIZE];

        public void SetValue(string message)
        {
            Encoding.UTF8.GetBytes(message).CopyTo(Msg, 0);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(Msg);
            return dataSource.ToArray();
        }
    }

    public class RoomChatResPacket
    {
        public UInt16 Result;
        
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomChatNtfPacket
    {
        public string UserID;
        public string Message;

        public bool FromBytes(byte[] bodyData)
        {
            UserID = Encoding.UTF8.GetString(bodyData, 0, PacketDef.MAX_USER_ID_BYTE_LENGTH);
            UserID = UserID.Trim();
            Message = Encoding.UTF8.GetString(bodyData, PacketDef.MAX_USER_ID_BYTE_LENGTH, PacketDef.MAX_CHAT_MSG_SIZE);
            Message = Message.Trim();

            return true;
        }
    }


     public class RoomLeaveResPacket
    {
        public UInt16 Result;
        
        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomLeaveUserNtfPacket
    {
        public Int64 UserUniqueId;

        public bool FromBytes(byte[] bodyData)
        {
            UserUniqueId = BitConverter.ToInt64(bodyData, 0);
            return true;
        }
    }


    
    public class RoomRelayNtfPacket
    {
        public Int64 UserUniqueId;
        public byte[] RelayData;

        public bool FromBytes(byte[] bodyData)
        {
            UserUniqueId = BitConverter.ToInt64(bodyData, 0);

            var relayDataLen = bodyData.Length - 8;
            RelayData = new byte[relayDataLen];
            Buffer.BlockCopy(bodyData, 8, RelayData, 0, relayDataLen);
            return true;
        }
    }


    public class PingRequest
    {
        public Int16 PingNum;

        public byte[] ToBytes()
        {
            return BitConverter.GetBytes(PingNum);
        }

    }
}
