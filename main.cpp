#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <array>
#include <stdlib.h>
#include "filehandler.h"

typedef unsigned int uint;
struct NewOrderStruct {
    static constexpr int kNewOrderMaxItems = 15;

    uint warehouse_id;
    uint district_id;
    uint customer_id;
    uint nr_items;

    uint ts_now;
    std::string SerializeTransactions();
    NewOrderStruct(std::string BackupString);

    NewOrderStruct(){};

    struct OrderDetail {
        uint item_id[kNewOrderMaxItems];
        uint supplier_warehouse_id[kNewOrderMaxItems];
        uint order_quantities[kNewOrderMaxItems];
        std::string SerializeTransactions();
        OrderDetail(std::string BackupString);
        OrderDetail(){};
    }detail;
};

struct DeliveryStruct {
    uint warehouse_id;
    uint o_carrier_id;
    uint32_t ts;
    std::string SerializeTransactions();
    DeliveryStruct(std::string BackupString);
    DeliveryStruct(){};
};

struct PaymentStruct {
    uint warehouse_id;
    uint district_id;
    uint customer_warehouse_id;
    uint customer_district_id;
    int payment_amount;
    uint32_t ts;
    uint customer_id;
    std::string SerializeTransactions();
    PaymentStruct(std::string BackupString);
    PaymentStruct(){};
};

DeliveryStruct GenerateDeliveryTransactions(int n){
    DeliveryStruct NewTransaction;
    NewTransaction.warehouse_id = (n+1)*1000;
    NewTransaction.o_carrier_id = (n+1)*1001;
    NewTransaction.ts = (n+1)*1002;

    return NewTransaction;
}

NewOrderStruct GenerateNewOrderTransactions(int n){
    NewOrderStruct NewTransaction;
    NewTransaction.warehouse_id = (n+1)*1000;
    NewTransaction.district_id = (n+1)*1001;
    NewTransaction.customer_id = (n+1)*1002;
    NewTransaction.nr_items = (n+1)*10;
    for(int i =0 ; i< 15; i++){
        NewTransaction.detail.item_id[i] = static_cast<uint>(n*45 + i + 1);
        NewTransaction.detail.supplier_warehouse_id[i] = static_cast<uint>(n*45 + 16 + i);
        NewTransaction.detail.order_quantities[i] = static_cast<uint>(n*45 + 31 + i);
    }
    return NewTransaction;
}

PaymentStruct GeneratePaymentTransactions(int n){
    PaymentStruct NewTransaction;
    NewTransaction.warehouse_id = (n+1)*1000;
    NewTransaction.district_id = (n+1)*1001;
    NewTransaction.customer_warehouse_id = (n+1)*1002;
    NewTransaction.customer_district_id = (n+1)*1002;
    NewTransaction.customer_id = (n+1)*1002;

    NewTransaction.payment_amount = (n+1)*1002;
    NewTransaction.ts = (n+1)*10;

    return NewTransaction;
}

class NewOrderState{
    int a;
    int b;
    int c;
    NewOrderState()
            : a(0) , b(1) , c(2) {}

};

class PaymentState{
    int a;
    int b;
    int c;
    PaymentState()
            : a(0) , b(1) , c(2) {}

};

class DeliveryState{
    int a;
    int b;
    int c;
    DeliveryState()
            : a(0) , b(1) , c(2) {}

};

class BaseTxn{
public:
    int serialid;
    int epoch_nr;
    virtual std::string SerializeTransaction()=0;
    BaseTxn(int serialid)
            : serialid(serialid)
    {}


};

std::string DeliveryStruct::SerializeTransactions()
{   std::string Type = "Delivery";
    return (Type + ','
            +std::to_string(warehouse_id)+','
            + std::to_string(o_carrier_id) + ','
            + std::to_string(ts)
    );
}
DeliveryStruct::DeliveryStruct(std::string BackupString) {

    std::istringstream iss(BackupString);
    std::vector<std::string> Parameters;
    std::string temp;
    while (getline(iss,temp, ',')) {
        Parameters.push_back(temp);
    }
    warehouse_id = static_cast<uint>(std::stoul(Parameters[1]));
    o_carrier_id = static_cast<uint>(std::stoul(Parameters[2]));
    ts = static_cast<uint32_t>(std::stoul(Parameters[3]));


}

std::string PaymentStruct::SerializeTransactions()
{
    std::string Type = "Payment";
    return (Type + ','
            + std::to_string(warehouse_id)+','
            + std::to_string(district_id) + ','
            + std::to_string(customer_warehouse_id) +','
            + std::to_string(customer_district_id) +','
            + std::to_string(payment_amount) +','
            + std::to_string(ts) +','
            + std::to_string(customer_id)
    );
}

PaymentStruct::PaymentStruct(std::string BackupString) {

    std::istringstream iss(BackupString);
    std::vector<std::string> Parameters;
    std::string temp;
    while (getline(iss,temp, ',')) {
        Parameters.push_back(temp);
    }
    warehouse_id = static_cast<uint>(std::stoul(Parameters[1]));
    district_id = static_cast<uint>(std::stoul(Parameters[2]));
    customer_warehouse_id = static_cast<uint>(std::stoul(Parameters[3]));
    customer_district_id = static_cast<uint>(std::stoul(Parameters[4]));
    payment_amount= static_cast<int>(std::stoul(Parameters[5]));
    ts = static_cast<uint32_t>(std::stoul(Parameters[6]));
    customer_id = static_cast<uint>(std::stoul(Parameters[7]));




}

template <typename TxnState>
class Txn: public BaseTxn{
public:
    Txn(int serialid)
            : BaseTxn(serialid){}

};

class NewOrderTxn;
class Client;


class DeliveryTxn : public Txn<DeliveryState> , public DeliveryStruct{
    Client *client;

public:
    std::string SerializeTransaction() final override;
    DeliveryTxn(Client *client, int serialID, int n)
            : Txn<DeliveryState>(serialID),
              DeliveryStruct(GenerateDeliveryTransactions(n)),
              client(client)
    {}
    DeliveryTxn(Client *client, int serialID, std::string BackupString)
            : Txn<DeliveryState>(serialID),
              DeliveryStruct(DeliveryStruct(BackupString)),
              client(client)
    {}

};

class PaymentTxn : public Txn<PaymentState> , public PaymentStruct{
    Client *client;

public:
    std::string SerializeTransaction() final override;
    PaymentTxn(Client *client, int serialID, int n)
            : Txn<PaymentState>(serialID),
              PaymentStruct(GeneratePaymentTransactions(n)),
              client(client)
    {}
    PaymentTxn(Client *client, int serialID, std::string BackupString)
            : Txn<PaymentState>(serialID),
            PaymentStruct(PaymentStruct(BackupString)),
            client(client)
    {}

};

class NewOrderTxn : public Txn<NewOrderState> , public NewOrderStruct{
    Client *client;

public:
    std::string SerializeTransaction() final override;
    NewOrderTxn(Client *client, int serialID, int n)
            : Txn<NewOrderState>(serialID),
              NewOrderStruct(GenerateNewOrderTransactions(n)),
              client(client)
    {}
    NewOrderTxn(Client *client, int serialID, std::string BackupString)
            : Txn<NewOrderState>(serialID),
              NewOrderStruct(NewOrderStruct(BackupString)),
              client(client)
    {}

};

std::string DeliveryTxn::SerializeTransaction(){
    return this->DeliveryTxn::SerializeTransactions();
}

std::string PaymentTxn::SerializeTransaction(){
    return this->PaymentTxn::SerializeTransactions();
}

std::string NewOrderTxn::SerializeTransaction(){
    return this->NewOrderTxn::SerializeTransactions();
}

class EpochClient{


    //int GenerateSerialId(int epoch_nr, int sequence);

public:
    FileHandler *BackSnapshot;
    BaseTxn *all_txns[20];
    EpochClient(){
        BackSnapshot = new FileHandler("Backup");
    };
    virtual BaseTxn *CreateTxn(int serial_id, int n) = 0;
    virtual BaseTxn *ReCreateTxn(int serial_id, std::string BackupSnapshot)=0;
    void GenerateBenchmarks();
    void RecreateTransactionsFromBackup(std::string *BackupSnapshot);
    virtual std::string SerializeAllTransactions() = 0;
    int GenerateSerialId(int epoch_nr, int sequence);
};



class Client: public EpochClient{
public:
    int rad;
    BaseTxn *CreateTxn(int serial_id,int n) final override;
    std::string SerializeAllTransactions() final override;
    BaseTxn *ReCreateTxn(int serial_id, std::string BackupSnapshot) final override;
    Client(){};


};




int EpochClient::GenerateSerialId(int epoch_nr, int sequence)
{
    return epoch_nr*100 + sequence;
}

void EpochClient::GenerateBenchmarks()
{
    //all_txns = new ;
    //BaseTxn *temp;
    //temp= new NewOrderTxn(this,123,1);
    //all_txns = temp;
    int t=0;
    for(int i = 0; i < 4; i++){
        for(int j=0; j < 5; j++){

    //BaseTxn::g_cur_numa_node = t / mem::kNrCorePerNode;
            all_txns[t++] = CreateTxn(GenerateSerialId(i,j),t);
    //all_txns = new NewOrderTxn(this,3724,0);
    //ptr = CreateTxn(GenerateSerialId(i, j),i*5+j);
    //ptr++;
        }
    }
}

void EpochClient::RecreateTransactionsFromBackup(std::string *BackupSnapshot) {
    int t=0;
    for(int i = 0; i < 4; i++){
        for(int j=0; j < 5; j++){

            //BaseTxn::g_cur_numa_node = t / mem::kNrCorePerNode;
            all_txns[t] = ReCreateTxn(GenerateSerialId(i,j),BackupSnapshot[t]);
            t++;
            //all_txns = new NewOrderTxn(this,3724,0);
            //ptr = CreateTxn(GenerateSerialId(i, j),i*5+j);
            //ptr++;
        }
    }
}
BaseTxn *Client::ReCreateTxn(int serial_id, std::string BackupSnapshot)
{
    std::string Type;
    std::istringstream iss(BackupSnapshot);
    std::getline(iss,Type,',');
    if(Type == "Delivery"){
        DeliveryTxn *Transaction;
        Transaction = new DeliveryTxn(this, serial_id, BackupSnapshot);
        return Transaction;
    }
    else if(Type == "Payment"){
        PaymentTxn *Transaction;
        Transaction = new PaymentTxn(this, serial_id, BackupSnapshot);
        return Transaction;
    }
    else{
        NewOrderTxn *Transaction;
        Transaction = new NewOrderTxn(this, serial_id, BackupSnapshot);
        return Transaction;
    }
}

BaseTxn *Client::CreateTxn(int serial_id,int n)
{   if(n%3==1) {
        NewOrderTxn *Transaction;
        Transaction = new NewOrderTxn(this, serial_id, n);
        return Transaction;
    }
    else if((n%3==2)){
        PaymentTxn *Transaction;
        Transaction = new PaymentTxn(this, serial_id, n);
        return Transaction;
    } else
    {
        DeliveryTxn *Transaction;
        Transaction = new DeliveryTxn(this, serial_id, n);
        return Transaction;
    }
}

std::string Client::SerializeAllTransactions(){
    auto ptr = all_txns;
    for(int i=0; i< 20 ; i++){
    //ptr++;
        BackSnapshot->AppendToFile(all_txns[i]->SerializeTransaction());
        //std::cout<<all_txns[i]->SerializeTransaction()<<'\n';
    }
    std::string test ="Hello";
    return test;
    //}

}





std::string NewOrderStruct::SerializeTransactions()
{ std::string Type = "NewOrder";
    return (Type + ','
            + std::to_string(warehouse_id)+ ','
            + std::to_string(district_id) + ','
            + std::to_string(customer_id) + ','
            + std::to_string(nr_items) + ','
            + detail.SerializeTransactions()
    );
}

std::string NewOrderStruct::OrderDetail::SerializeTransactions()
{
    std::string Items;
    std::string SupplierWarehouse;
    std::string OrderQuantities;
    for(int i = 0; i<kNewOrderMaxItems; i++){
        if(i==0){
            Items += std::to_string(this->item_id[i]);
            SupplierWarehouse += std::to_string(this->supplier_warehouse_id[i]);
            OrderQuantities += std::to_string(this->order_quantities[i]);
        }else{
            Items += ';'+ std::to_string(this->item_id[i]) ;
            SupplierWarehouse += ';'+ std::to_string(this->supplier_warehouse_id[i]);
            OrderQuantities += ';'+ std::to_string(this->order_quantities[i]);
        }

    }
    return (
            Items + '&'
            + SupplierWarehouse + '&'
            + OrderQuantities + '&'
    );
}

NewOrderStruct::OrderDetail::OrderDetail(std::string BackupString)
{
    std::istringstream iss(BackupString);
    std::vector<std::string> Parameters;
    std::string temp;
    while (getline(iss,temp, '&')) {
        Parameters.push_back(temp);
    }
    std::string Items = Parameters[0];
    std::string SupplierWarehouse = Parameters[1];
    std::string OrderQuantities = Parameters[2];
    std::istringstream iss1(Items);
    std::istringstream iss2(SupplierWarehouse);
    std::istringstream iss3(OrderQuantities);
    for(int i =0; i<kNewOrderMaxItems ; i++ ){
        std::string temp;
        getline(iss1,temp, ';');
        this->item_id[i] = static_cast<uint>(std::stoul(temp));
        getline(iss2,temp, ';');
        this->supplier_warehouse_id[i] = static_cast<uint>(std::stoul(temp));
        getline(iss3,temp, ';');
        this->order_quantities[i]  = static_cast<uint>(std::stoul(temp));


    }
}

NewOrderStruct::NewOrderStruct(std::string BackupString) {

    std::istringstream iss(BackupString);
    std::vector<std::string> Parameters;
    std::string temp;
    while (getline(iss,temp, ',')) {
        Parameters.push_back(temp);
    }
    warehouse_id = static_cast<uint>(std::stoul(Parameters[1]));
    district_id = static_cast<uint>(std::stoul(Parameters[2]));

    customer_id = static_cast<uint>(std::stoul(Parameters[3]));
    nr_items = static_cast<int>(std::stoul(Parameters[4]));

    detail = NewOrderStruct::OrderDetail(Parameters[5]);



}



void printTransaction(NewOrderStruct TR){
    std::cout << "Warehouse ID: " << TR.warehouse_id << "\n";
    std::cout << "District ID: " << TR.district_id << "\n";
    std::cout << "Customer ID: " << TR.customer_id << "\n";
    std::cout << "Number of Items: " << TR.nr_items << "\n";
    std::cout << "Item IDs: "<<"\n";
    for(int i = 0; i < 15; i++){
        std::cout << '\t' << TR.detail.item_id[i] <<'\n';
    }
    std::cout << "Supplier Warehouse IDs: "<<"\n";
    for(int i = 0; i < 15; i++){
        std::cout << '\t' << TR.detail.supplier_warehouse_id[i] <<'\n';
    }
    std::cout << "Order Quantities: "<<"\n";
    for(int i = 0; i < 15; i++){
        std::cout << '\t' << TR.detail.order_quantities[i] <<'\n';
    }
}



int main()
{   Client *client;
    client = new Client();
    client->GenerateBenchmarks();
    client->SerializeAllTransactions();
    client->BackSnapshot->PrintContentOfFile();
    std::string *temp;
    temp=client->BackSnapshot->ReadFile();
    for(int i = 0; i < 20; i++){
        std::cout << temp[i] << "\n";
    }
    delete client;
    //Failure occurs here
    Client *newclient;
    newclient = new Client();
    newclient->RecreateTransactionsFromBackup(temp);
    newclient->SerializeAllTransactions();
    newclient->BackSnapshot->PrintContentOfFile();
    std::string *temp2;
    temp2 = newclient->BackSnapshot->ReadFile();
    int flag=0;
    for(int i=0; i < 20; i++){
        if(temp[i]!=temp2[i]){
            flag =1;
            break;
        }
    }
    if(flag==0){
        std::cout << '\n' << "Everything is Perfect";
    }
    else{
        std::cout << '\n' << "Something Wrong Please Check";
    }





    return 0;
}