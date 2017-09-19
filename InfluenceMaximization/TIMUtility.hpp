
#ifndef TIMUtility_hpp
#define TIMUtility_hpp

#include <stdio.h>
#include <cmath>

#include <stdlib.h>
#include <cstdio>
using namespace std;

//Most of this code is used from the source code of TIM - Influence Maximization: Near-Optimal Time Complexity Meets Practical Efficiency by Tang et al.
// Source code - https://sourceforge.net/projects/timplus/
// License GNU GENERAL PUBLIC LICENSE Version 3

double MgT(int u){
    return (double)2;
}

double algo2(int n, int k, int m){
    double lb=1/2.0;
    double c=0;
    while(true){
        int loop= (6 * log(n)  +  6 * log(log(n)/ log(2)) )* 1/lb  ;
        c=0;
        double sumMgTu=0;
        for(int i=0; i<loop; i++){
            int u=rand()%n;
            double MgTu=MgT(u);
            double pu=MgTu/m;
            sumMgTu+=MgTu;
            c+=1-pow((1-pu), k);
        }
        c/=loop;
        if(c>lb) break;
        lb /= 2;
    }
    return c * n;
}
double KptEstimation(int n, int k, int m)
{
    double ept=algo2(n, k, m);
    ept/=2;
    return ept;
}

double logcnk(int n, int k){
    double ans=0;
    for(int i=n-k+1; i<=n; i++){
        ans+=log(i);
    }
    for(int i=1; i<=k; i++){
        ans-=log(i);
    }
    return ans;
}

int calculateR(int n, double epsilon, double opt, int k) {
    return (8+2 * epsilon) * ( log(n) + log(2) +  n * logcnk(n, k) ) / ( epsilon * epsilon * opt);
}

#endif /* TIMUtility_hpp */




