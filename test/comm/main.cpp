/*#include <cstdio>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "AES.h"
using namespace std;
 
 
//初始密钥
unsigned char key[16];
 
 
int main()
{
 
    ios::sync_with_stdio(false);
 
    cout << "请输入任意长度明文(按字符输入，最大长度不超过10^6，如'abcdgg125')：";
    cin >> P;
    cout << "请输入初始密钥(按字节输入，共16个字节.如'00 01 ... 0d 0e 0f')：";
    unsigned int xx;//作为“中间人”接收用户输入单字节密钥
    for(int i=0;i<16;i++)  // 输入密钥
    {
    	cin >> hex >> xx;
    	key[i]=xx;
    }
 
    cout << "输入的密钥是："<< endl;
    for(int i=0; i<16; i++) // 输入密钥
    {
        printf("%02x  ",key[i]);
    }
    cout  << "\n" << endl;
 
    KeyExpansion(key); //密钥扩展
 
    cout << "输入的明文为:" << endl;
    for(int i=0;i<strlen((char*)P);i++)
    {
        printf("%02x  ",P[i]);
    }
 
    cout << endl;
    encryption(); //加密
    cout << endl;
 
    cout << "加密后的密文为:" << endl;
    for(int i=0;i<strlen((char*)C);i++)
    {
        printf("%02x  ",C[i]);
    }
    cout  << "\n" << endl;
 
    decryption();  //解密
 
    cout << "解密后的明文为:" << endl;
    printf("%s\n",De_P);
 
    return 0;
}
*/