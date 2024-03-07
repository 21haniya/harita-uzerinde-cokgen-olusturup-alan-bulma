#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <graphics.h>
#include <curl/curl.h>

#define MAX_LINE 100
#define EKRAN_X 500
#define EKRAN_Y 500

#define SQUARE_SIZE 10


struct noktalar {
    int x;
    int y;
} koordinatlar[100];

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *p) {
    size_t total_size = size * nmemb;
    FILE *file = (FILE *)p;

    if (file) {
        fwrite(contents, 1, total_size, file);
        fwrite(contents, 1, total_size, stdout); 
    }

    return total_size;
}


int cokgenin_icinde(int x, int y, int dizi_cokgen[], int n) {
    int i, j;
    int c = 0;
    for (i = 0, j = n - 1; i < n; j = i++) {
        if ((dizi_cokgen[2 * i + 1] > y) != (dizi_cokgen[2 * j + 1] > y) &&
            (x < (dizi_cokgen[2 * j] - dizi_cokgen[2 * i]) * (y - dizi_cokgen[2 * i + 1]) / (dizi_cokgen[2 * j + 1] - dizi_cokgen[2 * i + 1]) + dizi_cokgen[2 * i]))
            c++;
    }
    
    return c%2==1;
}



int main() {
	
	FILE *file;
	
    CURL *curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Baslatma basarisiz\n");
        return EXIT_FAILURE;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "http://zplusorg.com/prolab.txt");
    

    file = fopen("koordinat.txt", "w");

    if (file == NULL) {
        fprintf(stderr, "Dosya acilamadi\n");
        return EXIT_FAILURE;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file); //ikisi beraber �al��r

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(stderr, "indirme problemi: %s\n", curl_easy_strerror(result));
    }
    
    curl_easy_cleanup(curl);
    fclose(file); 
    
    
    int koSayisi = 0;
    char filename[100] = "koordinat.txt"; 
    char karakter[MAX_LINE];
    int satir_oku = 0;

    printf("\nOkunacak satir: ");
    scanf("%d", &satir_oku);

    file = fopen(filename, "r");

    if (file == NULL) {
        printf("Dosya acma hatasi.\n");
        return 1;
    }
    
    //oku_devam true oldu�u s�rece okumaya devam edecek ve mevcut_satir izlenecek 
    bool oku_devam  = true;
    int mevcut_satir = 1;

    do {
        
        fgets(karakter, MAX_LINE, file);

        
        if (feof(file)) {
            oku_devam  = false;
            printf("Dosyada %d satir vardir.\n", mevcut_satir - 1);
            printf("%d. Satir bulunamadi\n", satir_oku);
        }
       
       //kullan�c�n�n arad��� sat�ra gelir
        else if (mevcut_satir == satir_oku) {
            oku_devam  = false;
            printf("Satir:\t%s", karakter);

            int i = 0;
            while (karakter[i] != '\0') {
                if (karakter[i] == '(') {
                    sscanf(karakter + i, "(%d,%d), ", &koordinatlar[koSayisi].x, &koordinatlar[koSayisi].y);
                    koSayisi++;
                }
                i++;
            }
        }

        mevcut_satir++;
    } while (oku_devam); 
    

    fclose(file);
    
    
    printf("Koordinat sayisi: %d\n", koSayisi);
    
    /*for (int i = 0; i < koSayisi; i++) {
        printf("%d. noktanin x=%d y=%d\n", i + 1, koordinatlar[i].x, koordinatlar[i].y);
    }*/

    
    initwindow(EKRAN_X, EKRAN_Y);
    setcolor(BLUE);
    //X ve Y koordinatlar�
    line(EKRAN_X/2, 0, EKRAN_X/2, EKRAN_Y);
    line(0, EKRAN_Y/2,EKRAN_X,EKRAN_Y/2);
    
    
    //grid yapma
    for(int i=0; i<EKRAN_X; i=i+10){
    	line(EKRAN_X/2, 0+i, EKRAN_X/2, 0+i);
    	line(0+i,EKRAN_Y/2, 0+i,EKRAN_Y/2 );
    	line(0+i,0,0+i,EKRAN_Y);
    	line(0,0+i,EKRAN_X,0+i);
    	
	}
	

    int ekranDizi[2 * koSayisi]; //ekran koordinatlar�n� bir diziye atma, drawpoly i�in gerekli
    int index = 0;

    for (int i = 0; i < koSayisi; i++) {
        
        ekranDizi[index] = koordinatlar[i].x * 10;
        index++;
        ekranDizi[index] = koordinatlar[i].y * 10;
        index++;
        
    }
		
	 for (int i = 0; i < koSayisi; i++) {
        for (int j = i + 1; j < koSayisi; j++) {
            if (koordinatlar[i].x == koordinatlar[j].x && koordinatlar[i].y == koordinatlar[j].y) {
                int koseSay = (j - i + 1) * 2;
                drawpoly(koseSay / 2, ekranDizi + i * 2);
                i = j;
            }
        }  
        
    }
    
    
	
	
    int kareSay = 0;
    
     for (int i = 0; i < EKRAN_X; i += SQUARE_SIZE) {
        for (int j = 0; j < EKRAN_Y; j += SQUARE_SIZE) {
        	
        	    int kareSolUstX = i ;
                int kareSolUstY = j ;
                int kareSagAltX = i + SQUARE_SIZE ;
                int kareSagAltY = j + SQUARE_SIZE ;
                int kareSolAltX=i ;
                int kareSolAltY=j+SQUARE_SIZE;
                
                
          
            if (cokgenin_icinde(kareSolUstX, kareSolUstY, ekranDizi, koSayisi) || 
			    cokgenin_icinde(kareSagAltX, kareSagAltY, ekranDizi, koSayisi) ||				
				cokgenin_icinde(kareSolAltX, kareSolAltY, ekranDizi, koSayisi)){
            	
                kareSay++;
                setfillstyle(1, WHITE); 
                bar(i, j, i + SQUARE_SIZE, j + SQUARE_SIZE); 
                
                
            }          
             
        }
    }
    
    for (int i = 0; i < koSayisi; i++){
    	
    	char str[20];
        setcolor(GREEN);
        sprintf(str, "(%d,%d)", koordinatlar[i].x, koordinatlar[i].y);
        outtextxy(koordinatlar[i].x * 10, koordinatlar[i].y * 10, str); 
	}


    printf("Cokgenin icindeki birim kare sayisi: %d\n", kareSay);
    
    int rezerv=kareSay*10;
   
    printf("Rezerv Degeri: %d", rezerv);
    
    int brMaaliyet=0;
    
   /* do{
    	printf("lutfen birim sondaj maaliyet degeri giriniz[1-10]:");
    	scanf("%d\n", &brMaaliyet);
    	
    	if( brMaaliyet<0 || brMaaliyet>10){
    		printf("Girilen deger istenilen aralikta degil! Tekar deneyiniz!");
    		break;
		}
    	
    	
	}while(1);*/
		

    getch();
    closegraph();
    return EXIT_SUCCESS;

    
    return 0;
}




