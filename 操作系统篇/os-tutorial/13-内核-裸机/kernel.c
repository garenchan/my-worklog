/* 这将迫使我们创建一个内核入口函数，而不是跳转到内核的0X00位置 */
void dummy_test_entrypoint() {
}

void main() {
    char* video_memory = (char*) 0xb8000;
    *video_memory = 'X';
}
