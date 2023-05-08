# Buffer-Sharing-in-Multi-tenant-Database-Environment 
[ICPC 2023 Online Spring Challenge powered by Huawei](https://codeforces.com/contests/1813)  
rank 118(regrettably didn't get 800 EUR)
# 1.Overview of task demand
The task ask competitors to accomplish the optimal scheduling algorithm to make page-evict smallest,the difficult is when the buffer sharing is full,which user should be chosen and which block shold be removed.For this problem,I accomlish one scheduling algorithm based on shadow price and LRU-hot cold zone.
# 2.Solution
## 2.1 the algorithm to choose user to be dispatch
When the buffer size is full,we should choose one previous block to remove for the new block,firstly we can designate one user to be dispatched.Through analysing the Scoring function, we can find that SLA is the important indicator that measure the user experience.So we can calculate the derivative of the score function,then we can choose the user has the smallest derivative,which is based on [shadow price](https://www.investopedia.com/terms/s/shadowpricing.asp).
In practice,we should firstly accomplish the SLAbase algorithm,then we can caluculate the SLA compared with the page-evict of SLAbase algorithm.
## 2.2 the algorithm to choose block
After we choose the user to dispatch,we should select which block of the user should be evicted from the buffer.In this step,due to the time limit,I accomplish the LRU base algorithm.For every user's block,we user the LRU to record the block status.When one block should be removed,we can remove one specific block based on LRU theory.As an improvement,I divide the LRU to two zone(cold zone and hot zone),which refer the mySql concept of cold and hot data.When one data is visited more than ome time,it enter the hot zone.When one block should be removed,we firstly select the block from the cold.And the size of cold and hot zone can be pointed in the initialization.
# 3.Improvement
> In fact,for 2.2,you can accomplish other different algorithm like LFU,MFU,and choose the best algorithm for the given data is to simulate all of them, keep track of their hit rate, and switch to the best performing one.
