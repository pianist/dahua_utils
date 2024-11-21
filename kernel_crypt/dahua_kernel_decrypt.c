#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    unsigned char buf_key[32];

    FILE* f = fopen("Dahua_hi3518ev200_kernel_KEY.bin", "r");
    fread(buf_key, 32, 1, f);
    fclose(f);

    for (int i = 0; i < 32; i++)
    {
        unsigned char h_i = buf_key[i];
        printf("0x%02x%s", h_i, (i % 8) == 7 ? "\n" : " ");
    }
    printf("\n\n");

    AES_KEY my_key;
    unsigned char iv[AES_BLOCK_SIZE];
    int x = AES_set_decrypt_key(buf_key, 256, &my_key);
    printf("AES_set_decrypt_key ret = %d\n", x);

    struct stat f_in_stat;
    stat(argv[1], &f_in_stat);

    printf("file size is %lu\n", f_in_stat.st_size);

    FILE* f_in = fopen(argv[1], "r");
    FILE* f_out = fopen(argv[2], "w");

    unsigned cnt = 0;
    unsigned sz_read = -1;
    while (!feof(f_in))
    {
        memset(iv, 0x00, AES_BLOCK_SIZE);
        *(unsigned*)iv = cnt / 2;

        char b[512];
        char b2[512];
        memset(b, 0, 512);
        memset(b2, 0, 512);

        sz_read = fread(b, 1, 512, f_in);

        if ((cnt % 2) == 0)
        {
            printf("reead and decrypted %u\n", sz_read);
            AES_cbc_encrypt(b, b2, sz_read, &my_key, iv, AES_DECRYPT);

            fwrite(b2, 1, sz_read, f_out);
        }
        else
        {
            printf("reead and copyed %u\n", sz_read);
            fwrite(b, 1, sz_read, f_out);
        }
        cnt ++;
    }

    fclose(f_out);
    fclose(f_in);

    return 0;
}

