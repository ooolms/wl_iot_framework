/*
 * Парсинг команд для управляющей платы
*/

const int bufSize=1024;//размер буфера
char buffer[bufSize+1];//буфер
int bufIndex=0;//текущий размер буфера
char delim='|';//код 124
const int maxArgCount=10;//максимальное число аргументов

//обработка команд
void processCommand(char *cmd,char *args[],int argsCount)
{
    if(strcmp(cmd,"testOk")==0)
    {
        Serial.println("ok");
    }
    else if(strcmp(cmd,"testErr")==0)
    {
        Serial.println("err|epic fail с русским текстом");
    }
    else if(strcmp(cmd,"testLongCmd")==0)
    {
        delay(1000);
        Serial.println("sync");
        delay(1000);
        Serial.println("sync");
        delay(1000);
        Serial.println("ok");
    }
    Serial.print("info|Cmd: ");
    Serial.println(cmd);
    for(int i=0;i<argsCount;++i)
    {
        Serial.print("info|Arg: ");
        Serial.println(args[i]);
    }
}

//поиск разделителя дальше по строке
int findDelim(int startFrom)
{
    for(int i=startFrom;i<bufSize;++i)
    {
        if(buffer[i]==0)return -1;
        else if(buffer[i]==delim)return i;
    }
    return -1;
}

//разбор строки из Serial на команду и аргументы
void parseCommand()
{
    if(buffer[0]==0||buffer[0]==delim)
        return;
    
    int bufIter=0;
    char *cmd=buffer;
    char *args[maxArgCount];//аргументы
    for(int i=0;i<maxArgCount;++i)
        args[i]=0;
    for(int i=0;i<maxArgCount;++i)
    {
        bufIter=findDelim(bufIter+1);//ищем разделитель
        if(bufIter==-1)//больше нет
        {
            processCommand(cmd,args,i);
            return;
        }
        buffer[bufIter]=0;//заменяем разделитель на символ с кодом 0
        if(bufIter==(bufSize-1))//разделитель в последнем символе в буфере, игнорируем
        {
            processCommand(cmd,args,i);
            return;
        }

        //следующий аргумент будет после позиции разделителя
        ++bufIter;
        args[i]=&buffer[bufIter];
    }
}

void setup()
{
    memset(buffer,0,bufSize+1);
    bufIndex=0;
}

void loop()
{
    while(Serial.available())
    {
        if(bufIndex==bufSize)//переполнение буфера, эпик фейл
        {
            //сбрасываем буфер
            memset(buffer,0,bufSize+1);
            bufIndex=0;
        }
        char c=(char)Serial.read();
        if(c=='\n')//признак конца сообщения, обрабатываем
        {
            buffer[bufIndex]=0;
            parseCommand();
            memset(buffer,0,bufSize+1);
            bufIndex=0;
        }
        else//продолжаем накапливать буфер
        {
            buffer[bufIndex]=c;
            ++bufIndex;
        }
    }
}
 
