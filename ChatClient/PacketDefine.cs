using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace csharp_test_client
{
    class PacketDef
    {
        public const Int16 PACKET_HEADER_SIZE = 5;
        public const int MAX_USER_ID_BYTE_LENGTH = 33;
        public const int MAX_USER_PW_BYTE_LENGTH = 33;
        public const int MAX_CHAT_MSG_SIZE = 257;
    }

    public enum PACKET_ID : ushort
    {
        DEV_ECHO = 1,

        // 로그인
        LOGIN_REQ = 201,
        LOGIN_RES = 202,
                
        ROOM_ENTER_REQ = 206,
        ROOM_ENTER_RES = 207,        
        ROOM_NEW_USER_NTF = 208,
        ROOM_USER_LIST_NTF = 209,

        ROOM_LEAVE_REQ = 215,
        ROOM_LEAVE_RES = 216,
        ROOM_LEAVE_USER_NTF = 217,

        ROOM_CHAT_REQ = 221,
        ROOM_CHAT_RES = 222,
        ROOM_CHAT_NOTIFY = 223,
    }


    public enum ERROR_CODE : Int16
    {
        ERROR_NONE = 0,



        ERROR_CODE_USER_MGR_INVALID_USER_UNIQUEID = 112,

        ERROR_CODE_PUBLIC_CHANNEL_IN_USER = 114,

        ERROR_CODE_PUBLIC_CHANNEL_INVALIDE_NUMBER = 115,
    }
}
