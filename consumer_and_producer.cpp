#include<iostream>
#include<windows.h>
#include<time.h>
#include<string.h>
using namespace std;
CRITICAL_SECTION cs, cfull, cempty;
HANDLE emptyEvent, fullEvent;

class Buffer {
private:
    char buf[7][11];
    int tail, head;
public:
    void push(char str[11]) 
    {
        if(!isFull()){
            strcpy(buf[head], str);
            head=(head+1)%7;
            time_t crt;
            time(&crt);
            printf("Msg: %s added. Time: %s", str, ctime(&crt));
            //cout<<"head: "<<head<<", tail: "<<tail<<endl;
            printAll();
            //cout<<"pro empty: "<<empt<<endl;
            //cout<<"pro full: "<<full+1<<endl;
            
        }
        else{
            printf("fulllllllllllllllllllll\n");
        }
    }

    char *pop()
    {
        int t=tail;
        tail=(tail+1)%7;
        time_t c;
        time(&c);
        printf("Msg: %s consumed. Time: %s", buf[t], ctime(&c));
        //cout<<"head: "<<head<<", tail: "<<tail<<endl;
        printAll();
        //cout<<"csm empty: "<<empt+1<<endl;
        //cout<<"csm full: "<<full<<endl;
        return buf[t];
    
    }

    bool isFull(){
        if((head+1)%7==tail){
            return true;
        }
        else{
            return false;
        }
    }

    void printAll()
    {
        if(tail==head){
            printf("queue empty.\n");
        }
        else{
            for(int i=tail;i!=head;i=(i+1)%7){
                printf("%s, ", buf[i]);
            }
            printf("\n");
        }
    }

    Buffer(){
        tail=0;
        head=0;
    }
};
Buffer buf;

const int PRODUCE_MAX_NUM=12;
const int CONSUME_MAX_NUM=8;
int empt=6, full=0;

int P(int *a)
{
    return --*a;
}
int V(int *a)
{
    return ++*a;
}

DWORD WINAPI ProducerThread(void* a)
{
    int produceNow=0;
    //cout<<produceNow<<endl;
    while(1){
        if(produceNow>=PRODUCE_MAX_NUM){
            break;
        }
        produceNow++;

        EnterCriticalSection(&cempty);
        P(&empt);
        //cout<<"out empty: "<<empt<<endl;
        LeaveCriticalSection(&cempty);

        if(empt<0){
            WaitForSingleObject(emptyEvent, INFINITE);  //没有空位
        }

        EnterCriticalSection(&cs);
        char msg[11]={0};
        // printf("%s\n", msg);
        sprintf(msg,"msgNum%d",produceNow);
        buf.push(msg);
        LeaveCriticalSection(&cs);


        EnterCriticalSection(&cfull);
        V(&full);
        LeaveCriticalSection(&cfull);
        // cout<<"pro full: "<<full<<endl;
        // cout<<"out full: "<<full<<endl;
        if(full<=0){
            SetEvent(fullEvent);
        }
        Sleep(rand()%100+1);
    }
    return 0;
}
 
DWORD WINAPI ConsumerThread(void *a)
{
    int consumeNow=0;
    while(1){
        if(consumeNow>=CONSUME_MAX_NUM){
            break;
        }
        consumeNow++;


        EnterCriticalSection(&cfull);
        P(&full);
        //cout<<"l144 full: "<<full<<endl;
        LeaveCriticalSection(&cfull);
        if(full<0){
            WaitForSingleObject(fullEvent, INFINITE);
        }

        EnterCriticalSection(&cs);
        buf.pop();
        LeaveCriticalSection(&cs);


        EnterCriticalSection(&cempty);
        V(&empt); 
        //cout<<"l157 empty: "<<empt<<endl;
        LeaveCriticalSection(&cempty);

        if(empt<=0){
            SetEvent(emptyEvent);
        }
        Sleep(rand()%100+1);
    }
    return 0;
}
int main()
{
    InitializeCriticalSection(&cs);
    InitializeCriticalSection(&cempty);
    InitializeCriticalSection(&cfull);
    emptyEvent=CreateEvent(NULL, false, false, NULL);
    fullEvent=CreateEvent(NULL, false, false, NULL);
    HANDLE producer_con[5];
    for(int i=0;i<2;i++){
        producer_con[i]=CreateThread(NULL, 0, ProducerThread, NULL, 0, NULL);
    }
    // HANDLE consumer[3];
    for(int i=2;i<5;i++){
        producer_con[i]=CreateThread(NULL, 0, ConsumerThread, NULL, 0, NULL);
    }
    WaitForMultipleObjects(5, producer_con, TRUE, INFINITE);
    // WaitForMultipleObjects(3, consumer, TRUE, INFINITE);
}