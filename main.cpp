#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <ctime>

using namespace std;

class ConcurrentResource
{
    pthread_mutex_t	mutex;

    public:

    ConcurrentResource()
    {
        resource = 0;
        mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    ~ConcurrentResource();

    volatile int resource;
    void add(int x);
    bool TakeOneIfExists();
};

ConcurrentResource::~ConcurrentResource()
{
    pthread_mutex_destroy(&mutex);
}

int roll_K6_dice()
{
    int x;
    x = rand() % 6;
    return x;
}

void ConcurrentResource::add(int x)
{
    pthread_mutex_lock(&mutex);
        resource += x;
    pthread_mutex_unlock(&mutex);
}


bool ConcurrentResource::TakeOneIfExists()
{
    bool result = false;
    pthread_mutex_lock(&mutex);
        if(resource > 0)
        {
            resource--;
            result = true;
        }
    pthread_mutex_unlock(&mutex);
    return result;
}


ConcurrentResource jedzenie;
ConcurrentResource zwierzyna;

void *watekMysliwego(void *_result){
    bool* result = (bool*)_result;
    int hunterLack = roll_K6_dice();
    int animalLack = roll_K6_dice();

    if(hunterLack > animalLack){
        zwierzyna.add(1);
    }
    if(jedzenie.TakeOneIfExists()){
        *result = true; // mysliwy przezyl
    }else{
        *result = false; //mysliwy umiera, nie bylo dla neigo jedzenie
    }
    pthread_exit(NULL);
}

void *watekKucharza(void *_result){
    bool* result = (bool*)_result;
    if(zwierzyna.TakeOneIfExists()){
        int n = roll_K6_dice();
        jedzenie.add(n);
    }
    if(jedzenie.TakeOneIfExists()){
        *result = true; // mysliwy przezyl
    }else{
        *result = false; //mysliwy umiera, nie bylo dla neigo jedzenie
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int liczba_mysliwych = atoi(argv[1]), liczba_kucharzy = atoi(argv[2]);
    int liczba_zwierzyny = atoi(argv[3]), liczba_jedzenia = atoi(argv[4]);



    pthread_t* threads_of_hunters = new pthread_t[liczba_mysliwych];
    pthread_t* threads_of_cooks = new pthread_t[liczba_kucharzy];
    bool* results = new bool[liczba_mysliwych + liczba_kucharzy];


    jedzenie.add(liczba_jedzenia);
    zwierzyna.add(liczba_zwierzyny);

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < liczba_mysliwych; j++){
                void* x = results;
            pthread_create(&threads_of_hunters[j], NULL, watekMysliwego, (void*)(results + j));
        }

        for (int k = 0; k < liczba_kucharzy; k++){
            pthread_create(&threads_of_cooks[k], NULL, watekKucharza, (void*)&results[liczba_mysliwych + k]);
        }

        for (int j = 0; j < liczba_mysliwych; j++){
            pthread_join(threads_of_hunters[j], NULL);
            if (results[j] == false){
                liczba_mysliwych -= 1;
            }
        }

        for (int k = 0; k < liczba_kucharzy; k++){
            pthread_join(threads_of_cooks[k], NULL);
            if (results[liczba_mysliwych + k] == false){
                liczba_kucharzy -= 1;
            }
        }

        cout<<"Obecnie w osadzie mieszka:"
            << "\nkucharzy:  \t" << liczba_kucharzy
            << "\nmysliwych: \t" << liczba_mysliwych
            << "\nzwierzyny: \t" << zwierzyna.resource
            << "\njedzenia:  \t" << jedzenie.resource
            << "\n-------------------------------------\n";

    }

    return 0;
}

