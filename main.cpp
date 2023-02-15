#include <stdio.h>      //necessary libraries
#include <pthread.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <queue>

using namespace std;


//struct of clients
struct Client {
    int client_id;
    int sleep_time;
    int machine_id;
    string company_type;
    int amount;
};

pthread_mutex_t machine_mut[10] = PTHREAD_MUTEX_INITIALIZER;        //initialize machine mutex
pthread_mutex_t write_file = PTHREAD_MUTEX_INITIALIZER;             //initialize write file mutex

int client_num;         // number of client


//total money in companies vallet
int kevin = 0;
int bob = 0;
int stuart = 0;
int otto = 0;
int dave = 0;

string arg;     //argv[1] to write in machine threads


Client clients[300]; // Client adds to this array, max 300 Client



void *machine_func (void *param);   //machine thread function
void *client_func(void *param);     //client thread function


int main(int argc, char *argv[]) {
    // read the file
    ifstream input_file(argv[1]);
    arg = argv[1];
    input_file >> client_num; // assign number of Client nmber

    //client and machine thread ids list
    pthread_t tid_client[client_num];
    pthread_t tid_machine[10];

    //create null client
    Client create;
    create = {0,0,0,"",0};
    clients[0]= create;
    for(int i=1; i<client_num+1;i++){
        string sleep_time;
        string machine_id;
        string company_type;
        string amount;
        string temp;

        //parse information coming from input
        create.client_id=i;                     //client id
        getline(input_file,sleep_time,',');
        create.sleep_time=stoi(sleep_time);     //client sleep time

        getline(input_file,machine_id,',');
        create.machine_id=stoi(machine_id);     //client machine id

        getline(input_file,company_type,',');
        create.company_type=company_type;       //client company type


        getline(input_file,amount,'\n');        
        create.amount=stoi(amount);             //client price amount 

        clients[i]=create;
    }

    //create clients
    for(int i=1 ; i<client_num+1; i++){
        pthread_create(&tid_client[i],NULL,client_func,&clients[i]);
    }
    //wait to sencronize
    for(int i=1; i<client_num+1; i++){
        pthread_join(tid_client[i],NULL);
    }


    // open output file
    string in(arg);
    in = in.substr(0, in.find("."));
    string out = in + "_log.txt";
    ofstream output_file; 

    output_file.open(out, ofstream::app); 

    //write total amount of money for companies
    output_file<<"All payments are completed"<<endl;
    output_file<<"Kevin: " <<kevin <<"TL"<< endl;
    output_file<<"Bob: " <<bob <<"TL"<< endl;
    output_file<<"Stuart: " <<stuart <<"TL"<< endl;
    output_file<<"Otto: " <<otto <<"TL"<< endl;
    output_file<<"Dave: " <<dave <<"TL"<< endl;
    output_file.close();
    return 0;
}


//machine threads function
void *machine_func (void *param){
    // create client info and put information coming from client
    struct Client *machine_info = (struct Client *) param;
    // lock the mutex and start critical section
    pthread_mutex_lock(&write_file);

    int id = machine_info->client_id;
    clients[id].client_id = machine_info->client_id;
    clients[id].machine_id = machine_info->machine_id;
    clients[id].amount = machine_info->amount;
    clients[id].company_type = machine_info->company_type;
    //by looking company type, make changes in total money
    if(clients[id].company_type == "Kevin"){
        kevin = kevin + clients[id].amount;
    }
    else if(clients[id].company_type == "Bob"){
        bob = bob + clients[id].amount;
    }else if(clients[id].company_type == "Stuart"){
        stuart = stuart + clients[id].amount;
    }else if(clients[id].company_type == "Otto"){
        otto = otto + clients[id].amount;
    }else if(clients[id].company_type == "Dave"){
        dave = dave + clients[id].amount;
    }
    //write output in machine thread
    string in(arg);
    in = in.substr(0, in.find("."));
    string out = in + "_log.txt";
    ofstream output_file; 

    output_file.open(out, ofstream::app);
    output_file << "Customer" << machine_info->client_id << "," << machine_info->amount << "TL," << machine_info->company_type << endl;
    output_file.close();
    // unlock mutex endof  critical section
    pthread_mutex_unlock(&write_file);

    pthread_exit(0);
}




//client threads function
void *client_func(void *param) {
    struct Client *ClientInfo = (struct Client*) param;
    usleep((unsigned int) ClientInfo->sleep_time*1000);  //each thread sleeps according to input
    // lock thread
    pthread_mutex_lock(&machine_mut[ClientInfo->machine_id-1]);
    pthread_t machine_thread;
    Client machine_args;
    //information to send machine thread
    machine_args.client_id = ClientInfo->client_id;
    machine_args.machine_id = ClientInfo->machine_id;
    machine_args.company_type = ClientInfo->company_type;
    machine_args.amount = ClientInfo->amount;
    machine_args.sleep_time = ClientInfo->sleep_time;
    //machine thread is created
    pthread_create(&machine_thread,NULL,machine_func,&machine_args);
    // end of the critical section
    pthread_mutex_unlock(&machine_mut[ClientInfo->machine_id-1]);
    //thread will wait for server thread to finish
    pthread_join(machine_thread,NULL);
    pthread_exit(0);
}