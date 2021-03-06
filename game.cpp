#include "../include/game.h"
#include "../include/rand.h"
#include <cstring>
#include<cstdio>
#include<pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define SPEED 500000
#define PENAL_SIZE (17*24*sizeof(int))
#define CUBE_SIZE (3*3*sizeof(int))

pthread_mutex_t	mutex_lock;//////////////////////////////////////////////////////

int num = 0;

static int stop_flag;
//Ϊ��ֻ����srand����һ�Σ�����ʱ�����������������Ͳ����ظ�
static Rand r;

/////////////////////////////////////////////
void Game::printNextCube(Context* graph)
{
int x = 7;
int y = 19;
    int i,j;
   Cur c;
    CubePoint p;   

c.saveCur();
   c.moveCur(6,19);
   cout<<"next : ";
   c.resumeCur();

    int a[3][3] = {0};  //��ȡ��������
    memcpy(a,graph->getArray(),CUBE_SIZE);
    for(i = x; i < x+3; i++)
        for(j = y; j < y+3; j++)
        {       p.setLocate(i,j);
		p.setColor(CLEAR);
                p.printPoint();
            if(a[i - x][j - y] == 1)
            {

                p.setColor(graph->getColor());
                p.printPoint();
            }
        }
}

void Game::printHelep()
{
   	Cur c;
	c.saveCur();
   	c.moveCur(10,19);
   	cout<<"a: go to left" << endl;
c.resumeCur();
c.saveCur();
   	c.moveCur(11,19);
   	cout<<"d: go to right" << endl;
c.resumeCur();
c.saveCur();
   	c.moveCur(12,19);
   	cout<<"w: roll cube" << endl;
c.resumeCur();
c.saveCur();
   	c.moveCur(13,19);
   	cout<<"s: go to down" << endl;
c.resumeCur();
c.saveCur();
   	c.moveCur(14,19);
   	cout<<"p: run or pause" << endl;
c.resumeCur();
}

void Game::gameInit()
{
	printHelep();

	nextGraph = new Context(getShape());
	nextGraph->draw();
	nextGraph->setLocate(1,7);

	createCube();
}

MARK Game::getMark()
{
	return mark;
}
void Game::setMark( MARK mark )
{
	this->mark = mark;
}

Game::~Game()
{
	if( NULL != m_graph )
	{
		delete m_graph;
		m_graph = NULL;
	}

	if( NULL != nextGraph )
	{
		delete nextGraph;
		nextGraph = NULL;
	}
}

//////////////////////////////////////////////

Game::Game()
{
    m_graph = NULL;
    x = 1;
    y = 7;
    CubePoint p;
    int i;
    s.printMessage();

	mark = GAME_RUN;////////////////////////////////////////////////

    memset((void*)m_penal,0,PENAL_SIZE); 
	memset((void*)m_color,0,PENAL_SIZE);///////////////////
   for(i = 0; i < 24; i++)
   {
        p.setLocate(i,0);
        p.setColor(BLUE);
        p.printPoint();
        p.setLocate(i,16);
        p.setColor(BLUE);
        p.printPoint();
        m_penal[i][0] = 1;
        m_penal[i][16] = 1;
   }
   for(i = 0; i < 17; i++)
   {
        p.setLocate(23,i);
        p.setColor(BLUE);
        p.printPoint();
        p.setLocate(0,i);
        p.setColor(RED);
        p.printPoint();
        m_penal[23][i] = 1;
        m_penal[0][i] = 1;
   }
   /*�������ֵ�Ƿ�����
   for(i = 0; i < 24; i++)
   {
        for(int j = 0; j < 17; j++)
            cout << m_penal[i][j] <<" ";
        cout << endl;
   }
   */
   fflush(stdout);
}

char Game::getShape()
{
    char ch;
    switch(r.randNum(1,6))
    {
        case 1:ch = 'Z';break;
        case 2:ch = 'T';break;
        case 3:ch = 'O';break;
        case 4:ch = 'I';break;
        case 5:ch = 'L';break;
        default:
               cout<<"no this shape type"<<endl;
               ch = '\0';
               break;
    }
    return ch;
}

bool Game::erasePenal()
{
    int i,j;
    int b[3][3] = {0};  //��ȡ��������

    m_graph->printG(CLEAR);
    memcpy(b,m_graph->getArray(),CUBE_SIZE);
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
        {
            m_penal[i + x][j + y] -= b[i][j];
		m_color[i][j] = CLEAR;//////////////////////////
        }
    return true;
}

bool Game::recoverPenal()
{
    int i,j;
    int b[3][3] = {0};  //��ȡ��������

    memcpy(b,m_graph->getArray(),CUBE_SIZE);
    for(i = x; i < x + 3; i++)
        for(j = y; j < y + 3; j++)
        {
            m_penal[i][j] += b[i-x][j-y];
		m_color[i][j] = m_graph->getColor();///////////////////////////
        }
    return true;

}

bool Game::setPenal()
{
    int i,j;
    int b[3][3] = {0};  //��ȡ��������

    m_graph->getLocate(&x,&y);
    memcpy(b,m_graph->getArray(),CUBE_SIZE);
    /*����ȡ�����������Ƿ�����
    for(i = 0;i < 3; i++)
    {
        for(j = 0; j < 3; j++)
            cout<<b[i][j]<< " ";
        cout<<endl;
    }
    */
    for(i = x; i < x + 3; i++)
        for(j = y; j < y + 3; j++)
        {
            m_penal[i][j] += b[i-x][j-y];
            if(m_penal[i][j] > 1)
            {
                cout<<"game over"<<endl;
                //�ӷ���ͳ�����а��
                system("stty icanon echo");
                exit(0);
            }
        }
    return true;
}

void Game::createCube()
{
    	m_graph = nextGraph;
	setPenal();
	m_graph->printG(YELLOW);

	nextGraph = new Context(getShape());
	nextGraph->draw();
	nextGraph->setLocate(1,7);
	printNextCube(nextGraph);
    /*
   for(int i = 0; i < 24; i++)
   {
        for(int j = 0; j < 17; j++)
            cout << m_penal[i][j] <<" ";
        cout << endl;
   }
   */
}

void Game::move(int dir)
{

	if(GAME_RUN != mark)
		return;
    erasePenal();
pthread_mutex_lock(&mutex_lock);///////////////////////////////////////
    switch(dir)
    {
        case DOWN:
            if(false == isAttachBottom())
            {
                m_graph->move(DOWN);
                setPenal();
                m_graph->printG(YELLOW);
            }
            else
            {
                recoverPenal();
                m_graph->printG(YELLOW);
                erase();
                stop();
            }
            break;
        case LEFT:
            if(false == isAttachLeft())
            {
                m_graph->move(LEFT);
                setPenal();
                m_graph->printG(YELLOW);
            }
            else
            {
                recoverPenal();
                m_graph->printG(YELLOW);
            }
 
            break;
        case RIGHT:
            if(false == isAttachRight())
            {
                m_graph->move(RIGHT);
                setPenal();
                m_graph->printG(YELLOW);
            }
            else
            {
                recoverPenal();
                m_graph->printG(YELLOW);
            }
            break;
        default:
            break;
    }
pthread_mutex_unlock(&mutex_lock);/////////////////////////////////////
}
void Game::roll()
{
    //ȡ�������ֵ���ȷŵ�һ��������
    int i,j;
    int flag = 0;
    int b[3][3] = {0};  //��ȡ��������
    int temp[3][3] = {0};

    m_graph->getLocate(&x,&y);
    memcpy(b,m_graph->getArray(),CUBE_SIZE);
    erasePenal();
    //��ת����
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
        {
            temp[2-j][i] = b[i][j];
        }
    //�ж���ת���Ƿ��������غ�
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            if (temp[i][j] == 1 && m_penal[x + i][y + j] == 1)   
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
            break;
    }
    //������غ�����ת���飬��������ֵ
    if(flag == 0)
    {
        m_graph->roll();
    }
    setPenal();
    m_graph->printG(YELLOW);
}
void Game::stop()
{
    delete m_graph;
	m_graph = NULL;
    stop_flag = 1;
    createCube();
}

bool Game::isAttachBottom()
{
    int i,j;
    int cube_x,cube_y;
    int b[3][3] = {0};  //��ȡ��������
    int flag = false;

    m_graph->getLocate(&cube_x,&cube_y);
    memcpy(b,m_graph->getArray(),CUBE_SIZE);

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
           if (b[i][j] == 1 && m_penal[i + cube_x + 1][j + cube_y] == 1)
           {
                flag = true;
                break;
           }
        }
        if (flag == true)
            break;
    }
    return flag;
}
bool Game::isAttachLeft()
{
    int i,j;
    int cube_x,cube_y;
    int b[3][3] = {0};  //��ȡ��������
    int flag = false;

    m_graph->getLocate(&cube_x,&cube_y);
    memcpy(b,m_graph->getArray(),CUBE_SIZE);

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
           if (b[i][j] == 1 && m_penal[i + cube_x][j + cube_y - 1] == 1)
           {
                flag = true;
                break;
           }
        }
        if (flag == true)
            break;
    }
    return flag;

}
bool Game::isAttachRight()
{
    int i,j;
    int cube_x,cube_y;
    int b[3][3] = {0};  //��ȡ��������
    int flag = false;

    m_graph->getLocate(&cube_x,&cube_y);
    memcpy(b,m_graph->getArray(),CUBE_SIZE);

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
           if (b[i][j] == 1 && m_penal[i + cube_x][j + cube_y + 1] == 1)
           {
                flag = true;
                break;
           }
        }
        if (flag == true)
            break;
    }
    return flag;
}
void Game::erase()
{
   int i,j;
   int flag = 0;
   static int count = 0;
   for(i = 22; i > 0; i--)
   {
        for(j = 1; j < 16; j++)
        {
            if(m_penal[i][j] == 0)
            {
                flag = 1;
            }
        }
        if(flag == 0)
        {
            //�ӷ֣�
            count++;
            s.setScore(count);
            s.printMessage();

            //���������ͼ����������
            down(i);
            i++;
        }
        flag = 0;
   }
}
void Game::down(int level)
{
    int i,j;
    int flag = 1;

    for(i = level; i > 1; i--)
        for(j = 1; j < 16; j++)
        {
            m_penal[i][j] = m_penal[i - 1][j];
        }
    //ˢ�����
    CubePoint p;
    for(i = 1; i < 23; i++)
        for(j = 1; j < 16; j++)
        {
            if(m_penal[i][j] == 1)
            {
                p.setLocate(i,j);
                //p.setColor(YELLOW);
		p.setColor(m_color[i][j]);
                p.printPoint();
            }
            if(m_penal[i][j] == 0)
            {
                p.setLocate(i,j);
                p.setColor(CLEAR);
                p.printPoint();
            }
        }
}
void* listenKey(void *ptr)
{
    Game* ptrg = (Game*)ptr;
    char key;
    while(1)
    {
        system("stty -icanon -echo");
        key = getchar();
        system("stty icanon echo");
        switch(key)
        {
            case 'a':
                ptrg->move(LEFT);break;
            case 'd':
                ptrg->move(RIGHT);break;
            case 'w':
                ptrg->roll();break;
            case 's':
                //�ٽ�
                while(1)
                {
                    if(stop_flag == 1)
                    {
                        stop_flag = 0;
                        break;
                    }
                    ptrg->move(DOWN);
                }
                break;

            case 'p':
		if(GAME_RUN == ptrg->getMark())
			ptrg->setMark(GAME_PAUSE);
		else ptrg->setMark(GAME_RUN);
                break;

            default:
                break;
        }
    }
}

void mainMenu();
void gameMenu();
void checkPassword(string& password);
void checkUsername(string& name );

int main()
{
    mainMenu();
    /*
    pthread_t t1;
    pthread_mutex_init(&mutex_lock, NULL);
	system("clear");
    Game g;
    //g.createCube();
	g.gameInit();
    pthread_create(&t1,NULL,listenKey,(void*)(&g));

    while(1)
    {
        fflush(stdout);
        usleep(SPEED);
        g.move(DOWN);
    }
     */
	return 0;
}

void checkPassword(){
}

void checkUsername(){
}

void mainMenu(){
    cout << endl << endl;
    cout << "*****************************************" << endl;
    cout << "******Welcome To Tetris Game System******" << endl;
    cout << "*****************************************" << endl;
    cout << endl << endl;
    cout << "          Login    (1)" << endl;
    cout << "          Register (2)" << endl;
    cout << "          Exit     (3)" << endl;
    cout << endl;
    cout << endl;
    cout << "Enter your choice: ";
    cout << endl;
    int choice;
    cin >> choice;
    //check choice is legal or not
    while (choice != 1 or choice != 2 or choice != 3){
        cout << "Your choice is not valid!" << endl;
        cout << "Please make a valid choice: ";
        cin >> choice;
    }
    switch(choice){
        case 1:
            //LoginMenu
            string name, password;

            cout << "*****************************************" << endl;
            cout << "\t\tEnter your user name: " << endl;
            cin >> name;
            checkUsername(name);

            cout << "\t\tEnter your password: " << endl;
            cin >> password;
            checkPassword(password);

            gameMenu();

            break;
        case 2:
            //registerMenu
            //set username
            string name, password1, password2;
            cout << "*****************************************" << endl;
            cout << "\t\tSet your user name: " << endl;
            cout << "\t\t-User Name should be combination of less than 8 digits or letters";
            cin >> name;
            /*****************************************************/
            //check if the username is used by the other user or not
            /*****************************************************/
            checkUsername(name);

            //set password
            do {
                cout << "\t\tSet your password: " << endl;
                cout << "\t\t-Password should be combination of 8 digits or letters";
                cin >> password1;
                checkUsername(name);
                cout << "\t\t-Repeat your password:";
                cin >> password2;
            } while (password1 != password2);

            /*****************************************************/
            //save user's information
            /*****************************************************/
            gameMenu();

            break;
        case 3:
            //Exit
            return 0;
    }
}

void gameMenu(){
    cout << endl << endl;
    cout << "*****************************************" << endl;
    cout << "******************TETRIS******************" << endl;
    cout << "*****************************************" << endl;
    cout << endl << endl;
    cout << "          Play Game            (1)" << endl;
    cout << "          Game Instructions    (2)" << endl;
    cout << "          High Score           (3)" << endl;
    cout << "          Purchase             (4)" << endl;
    cout << "          Exit                 (5)" << endl;
    cout << endl;
    cout << endl;
    cout << "Enter your choice: ";
    cout << endl;
    int choice;
    cin >> choice;

    //check choice is legal or not
    while (choice != 1 or choice != 2 or choice != 3){
        cout << "Your choice is not valid!" << endl;
        cout << "Please make a valid choice: ";
        cin >> choice;
    }

    switch(choice){
        case 1:
            //Play Game
            pthread_t t1;
            pthread_mutex_init(&mutex_lock, NULL);
            system("clear");
            Game g;
            //g.createCube();
            g.gameInit();
            pthread_create(&t1,NULL,listenKey,(void*)(&g));
            /*
            while(1)
            {
                fflush(stdout);
                usleep(SPEED);
                g.move(DOWN);
            }
            */
            break;
        case 2:
            //Game Instructions
            cout <<
            break;
        case 3:
            //High Score
            break;
        case 4:
            //Purchase

            break;
        case 5:
            //Exit
            return 0;
    }
}
