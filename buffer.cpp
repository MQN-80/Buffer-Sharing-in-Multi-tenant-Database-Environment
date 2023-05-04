#include<iostream>
#include<vector>
#include<unordered_map>
#include<algorithm>
using namespace std;

struct bufferConstraint{
    int Qmin;
    int Qbase;
    int Qmax;
 
}buf;
struct user{
    int q;
    vector<pair<int,int>>priority;
    vector<pair<int,int>>basicLoss;//the number of page faults obtained by using the LRU algorithm
    vector<pair<int,int>>userLoss; //the number of page faults obtained by using the your algorithm
    vector<pair<double,int>>sla;   //Computed derivative comparison
    int maxPrior;
}_user;
/*
The design of LRU: the hash table of bidirectional linked list is used. When it is accessed, it is moved to the head of the table. 
If a new entry is added, it is moved to the head of the table and the tail element is removed
*/
struct DLinkedNode {
    int key, value,user;   //key indicates the block number of the corresponding memory area, and value indicates the block number of the database area corresponding to the user
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode(): key(0), value(0), prev(nullptr), next(nullptr),user(-2) {}
    DLinkedNode(int _key, int _value,int _user): key(_key), value(_value), prev(nullptr), next(nullptr),user(_user) {}
}nodeD;
class ListCache {
private:
    unordered_map<int, DLinkedNode*> cache;
    unordered_map<int,DLinkedNode*>hotCache;  //hotspot cache
    DLinkedNode* head;
    DLinkedNode* tail;
    DLinkedNode* headHot;
    DLinkedNode* tailHot;  
    int hotSize;           //hotspot num size
    int hotSum;            //hotspot current size 
public:
    ListCache(int _hotSize){
        // 使用伪头部和伪尾部节点
        head = new DLinkedNode();
        tail = new DLinkedNode();
        head->next = tail;
        tail->prev = head;
        headHot=new DLinkedNode();
        tailHot=new DLinkedNode();
        headHot->next=tailHot;
        tailHot->prev=headHot;
        hotSum=0;
        hotSize=_hotSize;  
    }
    //Update the block to the queue head
    void update(int key,int step){
        if(hotCache.count(key)!=0)   //If it is already in the hot zone, place it at the head of the hot zone line
        {
            DLinkedNode* node = hotCache[key];
            movetoHotHead(node);
        }
        else{    //Otherwise it's in the cold zone
           if(hotSum<hotSize){   //热区未满，移入热区
                DLinkedNode* node = cache[key];
                removeNode(node);  //将其从冷区移出
                cache.erase(node->key);
                addToHot(node);
                hotCache[node->key]=node;
                hotSum++;
           }
           else{
                DLinkedNode* node = tailHot->prev;
                removeNode(node);   //将其从热区队尾移出
                addToHead(node);   //将其移动到冷区队首
                hotCache.erase(node->key);  //将其从热区缓存中删除
                cache[node->key]=node;      //移动到冷区缓存
                DLinkedNode* _node = cache[key];   //获取要更新的元素
                removeNode(_node);
                addToHot(_node);
                cache.erase(_node->key);
                hotCache[_node->key]=_node;
           }
        }
    }
    void movetoHotHead(DLinkedNode* node){
        removeNode(node);
        addToHot(node);
    }
    void addToHot(DLinkedNode* node){
        node->prev = headHot;
        node->next = headHot->next;
        headHot->next->prev = node;
        headHot->next = node;
    }
    void put(int key, int value,int user,int step) {
        // 如果 key 不存在，创建一个新的节点
        DLinkedNode* node = new DLinkedNode(key, value,user);
        // 添加进哈希表
        cache[key] = node;
        // 添加至双向链表的头部
        addToHead(node);      
    }
    void addToHead(DLinkedNode* node) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }
    
    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
 
    void moveToHead(DLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }
    DLinkedNode* removeTail() {
        DLinkedNode* node = tail->prev;
        if(node==head){     
            node=tailHot->prev;
        }
        removeNode(node);
        cache.erase(node->key);
        nodeD.key=node->key;
        nodeD.value=node->value;
        nodeD.user=node->user;
        delete node;
        return &nodeD;
    }
    //通过key删除node，彻底删除
    void deleteNode(int key){
        if(cache.count(key)){
            auto node=cache[key];
            node->prev->next = node->next;
            node->next->prev = node->prev;
            cache.erase(key);
            delete node;
        }
        else{
            auto node=hotCache[key];
            node->prev->next = node->next;
            node->next->prev = node->prev;
            hotCache.erase(key);
            delete node;
        }
    }
}ss1(10);
class basicCache {
private:
    unordered_map<int, DLinkedNode*> cache;
    DLinkedNode* head;
    DLinkedNode* tail;
 
public:
    basicCache(void){
        // 使用伪头部和伪尾部节点
        head = new DLinkedNode();
        tail = new DLinkedNode();
        head->next = tail;
        tail->prev = head;
    }
    //更新该块到队头
    void update(int key){
        DLinkedNode* node = cache[key];
        moveToHead(node);
    }
    
    void put(int key, int value,int user) {
        if (!cache.count(key)) {
            // 如果 key 不存在，创建一个新的节点
            DLinkedNode* node = new DLinkedNode(key, value,user);
            // 添加进哈希表
            cache[key] = node;
            // 添加至双向链表的头部
            addToHead(node);
        }
       
    }
    void addToHead(DLinkedNode* node) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }
    
    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
 
    void moveToHead(DLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }
    DLinkedNode* removeTail() {
        DLinkedNode* node = tail->prev;
        removeNode(node);
        cache.erase(node->key);
        nodeD.key=node->key;
        nodeD.value=node->value;
        nodeD.user=node->user;
        delete node;
        return &nodeD;
    }
    //通过key删除node，彻底删除
    void deleteNode(int key){
        auto node=cache[key];
        node->prev->next = node->next;
        node->next->prev = node->prev;
        cache.erase(key);
        delete node;
    }
 
}ss2;
class LRUCache {
private:
    unordered_map<int, DLinkedNode*> cache;
    DLinkedNode* head;
    DLinkedNode* tail;
    unordered_map<int,DLinkedNode*>userFisrtCache;  //存储lru中第一个,first代表用户号，second代表该用户在内存中的第一个内存块
public:
    LRUCache(void){
        // 使用伪头部和伪尾部节点
        head = new DLinkedNode();
        tail = new DLinkedNode();
        head->next = tail;
        tail->prev = head;
    }
    //更新该块到队头
    void update(int key,int user,vector<int>&userPrior,vector<int>&bufferNum){
        DLinkedNode* node = cache[key];
        moveToHead(node,userPrior,user,bufferNum);
    }
    //尝试根据用户优先级往进插入
    void put(int key, int value,int user,vector<int>&userPrior,vector<int>&bufferNum) {
        if (!cache.count(key)) {
            // 如果 key 不存在，创建一个新的节点
            DLinkedNode* node = new DLinkedNode(key, value,user);
            // 添加进哈希表
            cache[key] = node;
            // 待改进，使用哈希表存储每个用户块开始的最早位置,限制最大移动次数(避免优先级大的全部移到最后)
            addToHead(node,userPrior,user,bufferNum);     
        }  
    }
    void addToHead(DLinkedNode* node,vector<int>&userPrior,int user,vector<int>&bufferNum) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
        return;
    }
    
    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
 
    void moveToHead(DLinkedNode* node,vector<int>&userPrior,int user,vector<int>&bufferNum) {
        removeNode(node);
        addToHead(node,userPrior,user,bufferNum);
    }
    DLinkedNode* removeTail() {
        DLinkedNode* node = tail->prev;
        removeNode(node);
        cache.erase(node->key);
        nodeD.key=node->key;
        nodeD.value=node->value;
        nodeD.user=node->user;
        delete node;
        return &nodeD;
    }
    //通过key删除node，彻底删除
    void deleteNode(int key){
        auto node=cache[key];
        node->prev->next = node->next;
        node->next->prev = node->prev;
        cache.erase(node->key);
        delete node;
 
    }
    //寻找可移除的最小值
    DLinkedNode* removeMin(vector<int>&bufferNum,vector<bufferConstraint>&bufferConstraints,vector<ListCache>&lruList){
        calculteSla(); //计算sla
        auto sla=_user.sla;
        sort(sla.begin(),sla.end());
        int user=0;
        for(int i=1;i<sla.size();i++){
            user=sla[i].second;
            if(bufferNum[user]>bufferConstraints[user].Qmin)break;
        }
        auto mid=lruList[user].removeTail();
        nodeD.key=mid->key;
        nodeD.value=mid->value;
        nodeD.user=mid->user;
        return &nodeD;
    }
    //计算最小值导数
    void calculteSla(){
        for(int i=1;i<_user.basicLoss.size();i++){
            double sla_mid=(double)max(_user.userLoss[i].first,_user.basicLoss[i].first)/(double)_user.basicLoss[i].first-1.0;
            _user.sla[i].first=sla_mid*(double)_user.priority[i].first;
        }
    }
 
}ss;
class basicLine{
public:
    void Init(vector<bufferConstraint>bufferConstraints,int N){
        basicList.emplace_back(ss2);
        bufferNum.push_back(0);
        bufferMax.push_back(0);
        for(int i=1;i<bufferConstraints.size();i++){
            basicCache lru;
            basicList.emplace_back(lru);
            bufferNum.push_back(0);
            bufferMax.push_back(bufferConstraints[i].Qbase);
        } 

    }
    void operation(int user,int block){
        if(userDisk[user][block]!=-1){
            basicList[user].update(userDisk[user][block]);   //更新其到队头
        }
        else{
            _user.basicLoss[user].first++;
            if(bufferNum[user]>=bufferMax[user]){  //大于等于缓冲区，需要替换
                auto node=basicList[user].removeTail();   //删除队尾
                userDisk[user][block]=node->key;  //修改该位置对应数据库块位置
                userDisk[user][node->value]=-1;  //将原先那块替换出去
                basicList[user].put(node->key,block,user);  //将新块加入原先的list
            }
            else{
                userDisk[user][block]=bufferNum[user];   //所占内存就是bufferNum
                basicList[user].put(bufferNum[user],block,user);
                bufferNum[user]+=1;
            }
        }
    }
    vector<vector<int>>userDisk;
private:
    vector<basicCache>basicList; //用于基线cache替换
    vector<int>bufferMax; //每个用户缓冲区的最大值
    vector<int>bufferNum; //每个用户占内存大小
};
class bufferSharing{
public:
    bufferSharing(){
        cin>>N>>Q>>q;
        _user.q=Q;
        _user.maxPrior=0;
        int mid;
        _user.priority.emplace_back(120,0);
        _user.basicLoss.emplace_back(-1,-1);
        _user.userLoss.emplace_back(-1,-1);
        _user.sla.emplace_back(-1.0,-1);
        userPrior.emplace_back(0);
        for(int i=1;i<=N;i++){
            cin>>mid;
            _user.maxPrior=max(_user.maxPrior,mid);  //获取最大优先级
            userPrior.emplace_back(mid);
            _user.priority.emplace_back(mid,i);
            _user.basicLoss.emplace_back(0,i);
            _user.userLoss.emplace_back(0,i);
            _user.sla.emplace_back(0.0,i);
        }
        for(int i=1;i<=N;i++){
            cin>>mid;
            database.emplace_back(mid);
        }
        int qmin,qbase,qmax;
        bufferConstraints.emplace_back(buf);
        lruList.emplace_back(ss1);
        for(int i=1;i<=N;i++){
            cin>>qmin>>qbase>>qmax;
            bufferConstraint qq={qmin,qbase,qmax};
            bufferConstraints.push_back(qq);
            ListCache lru((qmin+qbase)/2);  //初始化容量为该用户所能占用内存区的最大值
            lruList.push_back(lru);
        }
        //初始用户磁盘区
        userDisk.emplace_back(NULL);
        basic.userDisk.emplace_back(NULL);
        for(int i=0;i<database.size();i++){
            vector<int>data(database[i]+1,-1);  //k+1块，下标从1-n
            vector<int>data1(database[i]+1,-1);  //k+1块，下标从1-n
            userDisk.emplace_back(data);
            basic.userDisk.emplace_back(data1);
        }
        //初始内存使用区
        for(int i=0;i<=Q;i++){
            memoryUse.emplace_back(-1);
            memoryBlock.emplace_back(-1);
 
        }
        //初始化
        for(int i=0;i<=N;i++){
            bufferNum.emplace_back(0);   //初始化用户占用内存块数量
        }
        //basic=new basicLine();
        basic.Init(bufferConstraints,N);
    }
    //模拟基线模型，用于调优
    //对每条指令进行操作
    void operation(){
        int tenant,block;
        int current=0;
        while(q--){
            current++;
            cin>>tenant>>block;
            //首先查询是否已经分配
            if(userDisk[tenant][block]!=-1){
                //allCache.update(userDisk[tenant][block],tenant,userPrior,bufferNum);//将其移到队头
                lruList[tenant].update(userDisk[tenant][block],current);//将其移到队头
                cout<<userDisk[tenant][block]<<endl;
            }
            else{
                //假如自身已经达到buffer最大值，则只能替换自己的块,替换自己队尾那块
                if(bufferNum[tenant]>=bufferConstraints[tenant].Qmax){
                    auto node=lruList[tenant].removeTail();   //删除队尾
                    memoryBlock[node->key]=block;  //修改该位置对应数据库块位置
                    userDisk[tenant][block]=node->key;
                    userDisk[tenant][node->value]=-1;  //将原先那块替换出去
                    lruList[tenant].put(node->key,block,tenant,current);  //将新块加入原先的list
                    cout<<node->key<<endl;   //输出该块对应内存块
                    //allCache.deleteNode(node->key);  //从整体内存中移除
                    //allCache.put(node->key,block,tenant,userPrior,bufferNum);  //将新块加入整体内存
                    
                }
                else if(isFree>Q){  //移除内存中最早的块，且对应用户需满足大于min
                    auto node=allCache.removeMin(bufferNum,bufferConstraints,lruList);
                    int user=node->user;
                    //lruList[user].deleteNode(node->key);    //删除该块对应用户list中的块
                    lruList[tenant].put(node->key,block,tenant,current);  //将新内存块移到该用户lru队首
                    memoryUse[node->key]=tenant;  //该内存块更换所属用户
                    userDisk[user][node->value]=-1;  //原内存块被移除，数据库块转为-1
                    userDisk[tenant][block]=node->key;  //指定用户数据库块对应的内存块位置
                    memoryBlock[node->key]=block;
                    bufferNum[user]--;
                    bufferNum[tenant]++;
                    //allCache.put(node->key,block,tenant,userPrior,bufferNum);
                    cout<<node->key<<endl;
                }
                else{
                    memoryUse[isFree]=tenant; //修改内存所有权
                    memoryBlock[isFree]=block;  //修改内存对应块号
                    userDisk[tenant][block]=isFree;
                    bufferNum[tenant]++;
                    lruList[tenant].put(isFree,block,tenant,current); //将内存号和块号存入lru中进行管理
                    //allCache.put(isFree,block,tenant,userPrior,bufferNum);
                    cout<<isFree<<endl;
                    isFree++;
                }
                _user.userLoss[tenant].first++;
            }
            basic.operation(tenant,block);   //进行基线计算
            cout.flush();
            }
        }
private:
    int N;//用户人数
    int Q;//buffer总大小
    int q;//输入序列的数量
    vector<int>userPrior; //存储用户优先级，第i个元素是第i个用户的优先级
    //vector<pair<int,int>>priority;//用户优先级，第一个数代表其优先级，第二个数代表对应用户编号，按优先级从小到大排列
    vector<int>database;//用户数据库大小
    vector<vector<int>>userDisk;//用户对应的数据库大小,userDisk[i][j]代表第i个租户的第j块存放在内存位置，-1表示还未存放
    vector<bufferConstraint>bufferConstraints;//用户buffer的限制条件
    vector<int>memoryUse;//记录内存被谁占用
    vector<int>bufferNum;//每个用户已经占用的内存块数量
    vector<int>memoryBlock; //每块内存对应的数据库块位置
    vector<ListCache>lruList;//用于进行lru替换
    LRUCache allCache;  //总体lru替换
    int isFree=1;  //空闲指针
    basicLine basic;
};
int main(){
    bufferSharing* buffer=new bufferSharing();
    buffer->operation();
    return 0;
}