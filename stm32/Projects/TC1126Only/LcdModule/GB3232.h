// ------------------  ������ģ�����ݽṹ���� ------------------------ //
struct  typFNT_GB32                 // ������ģ���ݽṹ 
{
       unsigned char  Index[2];               // ������������	
       unsigned char   Msk[128];                        // ���������� 
};

/////////////////////////////////////////////////////////////////////////
// ������ģ��                                                          //
// ���ֿ�: ����16.dot,����ȡģ���λ,��������:�����Ҵ��ϵ���         //
/////////////////////////////////////////////////////////////////////////
const struct  typFNT_GB32 codeGB_32[] =          // ���ݱ� 
{

"��", 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x20,0x00,0x00,0x00,0x38,0x00,0x00,
      0x00,0x30,0x00,0x00,0x00,0x30,0x01,0xF0,
      0x00,0x30,0x3F,0x70,0x00,0x30,0x08,0x60,
      0x00,0x30,0x0C,0x60,0x00,0x3E,0x0C,0x60,
      0x00,0xF8,0x18,0x60,0x00,0x10,0x12,0xC0,
      0x00,0x10,0x31,0xC0,0x00,0x1E,0x61,0x80,
      0x00,0xF8,0x80,0x00,0x1F,0xB0,0x00,0xC0,
      0x3C,0x30,0x07,0xF0,0x02,0x3F,0x78,0x60,
      0x03,0x30,0x20,0x60,0x03,0x10,0x30,0x40,
      0x06,0x90,0x30,0x40,0x04,0x70,0x37,0xE0,
      0x0C,0x18,0x18,0x00,0x18,0x0E,0x10,0x00,
      0x30,0x03,0x80,0x00,0x40,0x01,0xF0,0x00,
      0x00,0x00,0x7F,0x00,0x00,0x00,0x3F,0xFE,
      0x00,0x00,0x0F,0xF0,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

"��", 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,
      0x00,0x03,0x80,0x00,0x00,0x03,0x80,0x00,
      0x00,0x03,0x00,0x00,0x00,0x03,0x00,0x00,
      0x00,0x03,0x00,0x00,0x00,0x03,0x00,0x00,
      0x00,0x03,0x00,0x00,0x00,0x03,0x00,0x00,
      0x00,0x03,0x0F,0x80,0x00,0x03,0xFF,0x80,
      0x03,0xFF,0x80,0x00,0x01,0xE3,0x00,0x00,
      0x00,0x03,0x00,0x00,0x00,0x03,0x00,0x00,
      0x00,0x06,0x80,0x00,0x00,0x06,0x80,0x00,
      0x00,0x06,0x40,0x00,0x00,0x0C,0x60,0x00,
      0x00,0x0C,0x30,0x00,0x00,0x18,0x38,0x00,
      0x00,0x18,0x1C,0x00,0x00,0x30,0x0E,0x00,
      0x00,0x60,0x0F,0x80,0x01,0x80,0x07,0xE0,
      0x02,0x00,0x03,0xFC,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

"��", 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x03,0x80,0x00,0x00,0x01,0xC0,0x00,
      0x00,0x00,0xC0,0x00,0x00,0x00,0x1F,0x80,
      0x00,0x83,0xF1,0xC0,0x01,0xFC,0x01,0xE0,
      0x01,0x80,0x03,0xC0,0x03,0x80,0x02,0x00,
      0x03,0x01,0xF0,0x00,0x03,0x3E,0x38,0x00,
      0x00,0x18,0x70,0x00,0x00,0x00,0x60,0x00,
      0x00,0x01,0xC0,0x00,0x00,0x01,0x80,0xF0,
      0x00,0x00,0xFF,0xF8,0x00,0x7F,0xF8,0x00,
      0x1F,0xF0,0xC0,0x00,0x0C,0x00,0x40,0x00,
      0x00,0x00,0x60,0x00,0x00,0x00,0x60,0x00,
      0x00,0x00,0x60,0x00,0x00,0x00,0x60,0x00,
      0x00,0x00,0x60,0x00,0x00,0x00,0xE0,0x00,
      0x00,0x18,0xC0,0x00,0x00,0x07,0xC0,0x00,
      0x00,0x03,0xC0,0x00,0x00,0x01,0x80,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};