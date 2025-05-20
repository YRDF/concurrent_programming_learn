#include<iostream>

class Single{
public:
    static Single& GetInstance(){
        static Single s1;
        return s1;
    }
private:
    Single(){}
    Single(const Single& s1) = delete;
    Single& operator= (const Single& s1) = delete;
};

void testSingel(){
    std::cout<<"s1 add is: "<<&Single::GetInstance()<<std::endl;
    std::cout<<"s2 add is: "<<&Single::GetInstance()<<std::endl;
}

int main(){
    std::cout<<"hello world!"<<std::endl;
    // Single s1;
    // Single s2;
    //testSingel();
    return 0;
}