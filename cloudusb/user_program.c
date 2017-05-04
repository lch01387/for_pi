#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
//#include <sys/types.h>
//#include <stdilb.h>

#define INIT 0
#define RETURN_FILE 1

typedef struct FILES{
    int pid;
    int *block_require_p; // 이 변수를 init과정에서 넘겨주고 signal로 불려지면 출력해볼것.
} fileS;

typedef struct return_fileS{
    char *buf;
} fileS;

fileS files;
int block_require = 0;
int fd = -1;

void file_transfer(int signo);

int main(){
    int ret = 0;
    
    /* 모든 구글API설정이 끝난 후 */
    signal(SIGCONT, file_transfer); //
    
    /* 커널모듈로 전송 */
    files.pid = getpid();
    printf("User Pid is %d\n", files.pid);
    files.block_require_p = &block_require;
    if((fd = open("/dev/CloudUSB", O_RDWR)) < 0 )
    {
        printf("Device Open failed!!\r\n");
        printf("%d\n", errno);
        return -1;
    }
    printf("구조체 주소: %p\n", &files);
    ret = ioctl(fd, INIT, &files);
    if(ret < 0)
        printf("Error in IOCTL errno: %d\r\n", errno);
    
    /* pause로 무한대기. signal이 들어오면 해당함수로 */
    while(1)
        pause();
    return 0;
}

/*
 * 시그널이 발생하면 실행될 함수.
 * 실행흐름이 이 함수로 넘어왔다가 다시 main함수로 돌아간다.
 * signo: 어떤 시그널에의해 발생했는지, 중요x.
 */
void file_transfer(int signo){
    int ret = 0;
    /* 파일을 전송하기 위해 구글로부터 다운로드 및 구조체 세팅 */
    printf("유저프로그램 시그널 전달받음\n");
    printf("block_require = %d\n", block_require);
    
    
    /* ioctl로 파일정보가 담긴 구조체 전달 */
    ret = ioctl(fd, RETURN_FILE, NULL);
    //ioctl(fd, RETURN_FILE, file)
}














