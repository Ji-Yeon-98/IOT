#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <iostream>
#include <string>
#include <cstring>

#define tact_d "/dev/tactsw"
#define DOT_DEV "/dev/dot"
#define CLCD "/dev/clcd"
#define led "/dev/led"
#define TIME_QUANTUM 1667
#ifndef COORD_SET

int tact;
int score = 0;
int rnd; // �迭������ ����
int hintcount = 3;	// ��ƮȽ�� 
unsigned char In;
using namespace std;

char rhint[4][32] = { "R : 22 6 6 22    22 6 6 22  ",
                     "R : 11 11 4 121   222 8 22 11 ",
                     "R : 1 1 6 11       11 6 1 1   ",
                     "R : 2 4 6 33       33 6 4 2   " };	// �� ��Ʈ 

char chint[4][32] = { "C : 0 8 8 22       22 8 8 0   ",
                     "C : 2 4 33 4       4 33 4 2   ",
                     "C : 1 1 6 11      11 6 1 1    ",
                     "C : 2 4 6 33      33 6 4 2    " };	// �� ��Ʈ 

bool matrixPaper[8][8] = { {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0} };  // �׸� �׸���� ��Ʈ����

bool matrixCursor[8][8] = { {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0} };  // Ŀ�� ǥ�ÿ� ��Ʈ����

bool matrixAns[4][8][8] = {
        {
            {0, 1, 1, 0, 0, 1, 1, 0},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 1, 0, 0, 1, 1, 0},
            {0, 1, 1, 0, 0, 1, 1, 0},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 1, 0, 0, 1, 1, 0}
        },
        {
            {0, 0, 1, 0, 0, 1, 0, 0},
            {0, 0, 1, 0, 0, 1, 0, 0},
            {0, 0, 1, 1, 1, 1, 0, 0},
            {0, 1, 0, 1, 1, 0, 1, 0},
            {1, 1, 0, 1, 1, 0, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1},
            {0, 1, 1, 0, 0, 1, 1, 0},
            {0, 0, 1, 0, 0, 1, 0, 0}
        },
        {
            {0, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 1, 1, 1, 1},
            {0, 0, 1, 0, 0, 1, 0, 0},
            {0, 0, 1, 0, 0, 1, 0, 0},
            {1, 1, 1, 1, 1, 1, 0, 0},
            {0, 0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 1, 0, 0}
        },
        {
            {0, 0, 0, 1, 1, 0, 0, 0},
            {0, 0, 1, 1, 1, 1, 0, 0},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 0, 0, 1, 1, 1},
            {1, 1, 1, 0, 0, 1, 1, 1},
            {0, 1, 1, 1, 1, 1, 1, 0},
            {0, 0, 1, 1, 1, 1, 0, 0},
            {0, 0, 0, 1, 1, 0, 0, 0}
        }
}; // ������ ��Ʈ����

typedef struct coord
{
    int y;
    int x;
} coord;
#endif

class DotMatrix	//��Ʈ ��Ʈ���� �Լ� 
{
private:
    int dot_fd;
    void openDot() { dot_fd = open(DOT_DEV, O_WRONLY); }
    void closeDot() { close(dot_fd); }

public:
    DotMatrix() // �� ��Ʈ������ ������
    {
        dot_fd = -1;
        memset(matrixPaper, 0, sizeof(bool) * 8 * 8);
        memset(matrixCursor, 0, sizeof(bool) * 8 * 8);
    }
    ~DotMatrix() { close(dot_fd); }

    // matrixPaper �ʱ�ȭ
    void clearPaper() { memset(matrixPaper, 0, sizeof(bool) * 8 * 8); }
    // matrixCursor �ʱ�ȭ
    void clearCursor() { memset(matrixCursor, 0, sizeof(bool) * 8 * 8); }

    // matrixPaper�� �׸��� �׸�
    void draw(coord C) { matrixPaper[C.y][C.x] = true; }
    // matrixPaper�� �׷��� �׸��� ����
    void erase(coord C) { matrixPaper[C.y][C.x] = false; }

    // Ŀ���� ��ġ�� ���
    void cursor(coord C) { matrixCursor[C.y][C.x] = true; }

    // matrixPaper�� �� ���
    void drawToPaper(int microSec)
    {
        openDot();
        unsigned char rows[8];
        for (int i = 0; i < 8; i++)
        {
            rows[i] = 0;
            for (int j = 0; j < 8; j++)
            {
                if (matrixPaper[i][j])
                {
                    rows[i] |= 1 << j;
                }
            }
        }
        write(dot_fd, &rows, sizeof(rows));
        usleep(microSec);
        closeDot();
    }

    // matrixCursor�� �� ���
    void drawToCursor(int microSec)
    {
        openDot();
        unsigned char rows[8];
        for (int i = 0; i < 8; i++)
        {
            rows[i] = 0;
            for (int j = 0; j < 8; j++)
            {
                if (matrixCursor[i][j])
                {
                    rows[i] |= 1 << j;
                }
            }
        }
        write(dot_fd, &rows, sizeof(rows));
        usleep(microSec);
        closeDot();
    }

    //���� ���, ���� 9�� ������ �� ���䵵 �����ش�.
    void drawToAns(int rnd)
    {
        openDot();
        unsigned char rows[8];
        for (int i = 0; i < 8; i++)
        {
            rows[i] = 0;
            for (int j = 0; j < 8; j++)
            {
                if (matrixAns[rnd][i][j])
                {
                    rows[i] |= 1 << j;
                }
            }
        }
        write(dot_fd, &rows, sizeof(rows));
        sleep(5);
        closeDot();
    }
};

class CharacterLCD	// CLCD �Լ� 
{
private:
    int clcd_fd;

    //CharacterLCD�� ���
    void print(string S)
    {
        char* cstr = new char[S.length() + 1];
        strcpy(cstr, S.c_str());
        clcd_fd = open(CLCD, O_WRONLY);
        write(clcd_fd, cstr, 32);
        close(clcd_fd);
    }

public:
    void beforeGame()	// ������ �����ϱ� ���� ��� 
    {
        string s1 = "   Nemo Logic   ";
        string s2 = " Ready to Start ";
        print(s1 + s2);
    }

    void rHint(int rnd)	// ����Ʈ ��� 
    {
        string s1 = rhint[rnd];
        print(s1);
    }
    void cHint(int rnd)	// ����Ʈ ��� 
    {
        string s1 = chint[rnd];
        print(s1);
    }

    void endGame(int score)	// ���� ��� 
    {
        string s1 = " score : ";
        char sc2[9];
        sprintf(sc2, "%d", score);
        string s2 = sc2;
        print(s1 + s2);
    }
};

class HLED
{
public :
	void hintl(int hled)
	{
		unsigned char data=0x00;
	 	if(hintcount==3){
	 		data=0x1f;
	 		write(hled, &data, sizeof(unsigned char));
		 	usleep(100000);
		 }
		 if(hintcount==2){
		 	data=0x3f;
		 	write(hled, &data, sizeof(unsigned char));
		 	usleep(100000);
		 }
		 if(hintcount==1){
		 	data=0x7f;
		 	write(hled, &data, sizeof(unsigned char));
		 	usleep(100000);
		 }
		 else{
		 	data=0xff;
		 	write(hled, &data, sizeof(unsigned char));
		 	usleep(100000);
		 }
		 	
	}
};

int main() {

    DotMatrix DM;
    CharacterLCD CL;
    HLED HL;

    coord ret = { 0,0 };	// ��Ʈ ��Ʈ���� ��ġ �ʱ�ȭ 
    rnd = rand() % 4;		// ���� �� ������ �������� ���� 
    
    int hintx=0, hinty=0;	// ��Ʈ�� ��Ʈ�� ų ��ǥ ��ġ 

    int hintled;
    hintled = open(led, O_RDWR);
    if(hintled < 0){
    	printf("open failed! \n");
	}
    
	tact = open(tact_d, O_RDWR);
    if (tact < 0) {
        printf("open failed! \n");
        return 0;
    }

    CL.beforeGame();
    sleep(5); //��⸦ �۵��ϰ� 5�� �ڿ� ���� ���� 
    CL.rHint(rnd);

    while (1) {
        switch (In) {
        case 1: // ����Ʈ ��� 
            CL.rHint(rnd);
            break;
        case 3: // ����Ʈ ��� 
            CL.cHint(rnd);
            break;

            //Ŀ���� �����̴� Ű 
        case 2: //UP
        	if(ret.y>0)
            	ret.y -= 1;
            break;
        case 8: //Down
        	if(ret.y<7)
            	ret.y += 1;
            break;
        case 4: //Left
        	if(ret.x<7)
            	ret.x += 1;
            break;
        case 6: //Right
        	if(ret.x>0)
            	ret.x -= 1;
            break;

        case 5: //Ok
            DM.draw(ret);
            break;

        case 9: //SCORE
            for (int i = 0; i < 8; i++)
            {
                // �������� �ۼ��� ��� �� 
                for (int j = 0; j < 8; j++)
                {
                    if (matrixPaper[i][j] == matrixAns[rnd][i][j])
                        score = score + 1;
                }
            }
            CL.endGame(score);
            DM.clearPaper();
            DM.drawToAns(rnd);
          //������ ���� ������ �� 5�� �� �����
            DM.clearPaper();
            score = 0;
            hintcount = 3; 
            ret = { 0,0 };
            rnd = rand() % 4;
            CL.rHint(rnd);  //���� ������ �� �����(�ٸ� ���� ����)
            break;

        case 10:	//�����
            DM.clearPaper();
            score = 0;
            hintcount = 3;
            ret = { 0,0 };
            rnd = rand() % 4;
            CL.rHint(rnd);
            break;
            
        case 11:	//��Ʈ �ֱ�
			if(hintcount>0){
				while(matrixPaper[hintx][hinty]!=0 || matrixAns[rnd][hintx][hinty]!=1){
					hintx=rand()%8;
					hinty=rand()%8;
				}
				matrixPaper[hintx][hinty]=1;
				hintcount--;
			}
			else
				hintcount=0;
				break; 


        case 12: 	//ERASE
            DM.erase(ret);
            break;
        }

        while (1) {
        	HL.hintl(hintled);
            DM.clearCursor();
            DM.cursor(ret);

            DM.drawToPaper(TIME_QUANTUM * 20);
            DM.drawToCursor(TIME_QUANTUM * 20);
            read(tact, &In, sizeof(In));
            usleep(100000); //1�� ������
            if (In) {
                break;
            }
        }

    }
    return 0;
}

//  matrixPaper : �׸� �׸��� ���� ������ 8*8 �迭
//  matrixCursor : Ŀ���� ��ġ�� ��Ÿ���� 8*8 �迭
//  clearPaper :  matrixPaper �ʱ�ȭ
//  clearCursor : matrixCursor �ʱ�ȭ
//  draw : Ŀ���� ��ġ�� ��ǥ�� ���� matrixPaper �迭��ǥ�� ���� true�� ���� = �׸��׸�
//  erase : Ŀ���� ��ġ�� ��ǥ�� ���� matrixPaper �迭��ǥ�� ���� false�� ���� = �׸�����
//  cursor : 8*8 �� ��Ʈ���� �󿡼� Ŀ���� ��ġ�� true�� ����, clearCursor�� while �ݺ��ϱ� ������ Ŀ���� ��ġ�� �Һ��� ����