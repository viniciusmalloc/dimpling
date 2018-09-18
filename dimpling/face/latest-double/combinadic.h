#ifndef COMBINADIC_H
    #define COMBINADIC_H
    typedef long long int64;
#endif

__device__ class Combination{
    private:
        int n, k;
        int data[4];
        int64 largestV(int, int, int64);

    public:
        __device__ Combination(int n, int k){
            if (n < 0 || k < 0){ // normally n >= k
                printf("Negative parameter in constructor\n");
                return;
            }

            this->n = n;
            this->k = k;
            for (int i = 0; i < k; i++)
                data[i] = i;
        }// Combination(n, k)

        __device__ Combination(int n, int k, int* a){ // Combination from a[]
            this->n = n;
            this->k = k;

            for (int i = 0; i < k; i++)
                data[i] = a[i];

            if (!isValid()){
                printf("Bad value from array!\n");
                return;
            }
        }// Combination(n, k, a)
        
        __device__ Combination(){
        }   

        __device__ static int64 choose(int n, int k){
            if (n < 0 || k < 0){
                printf("Invalid negative parameter in choose()\n");
                return -1;
            }
            if (n < k) return 0;  // special case
            if (n == k) return 1;

            int64 delta, iMax;

            if (k < n-k){ // ex: choose(100, 3)
                delta = n-k;
                iMax = k;
            }
            else{         // ex: choose(100, 97)
                delta = k;
                iMax = n-k;
            }

            int64 ans = delta + 1;

            for (int64 i = 2; i <= iMax; ++i)
                ans = (ans * (delta + i)) / i; 

            return ans;
        } // Choose()

        __device__ bool isValid();
        __device__ Combination successor();
        __device__ Combination element(int64);
        __device__ int* getArray();

};// Combination class

// return largest value v where v < a and choose(v, b) <= x
__device__ int64 Combination::largestV(int a, int b, int64 x){
    int v = a - 1;
           
    while (choose(v, b) > x) --v;

    return v;
}// LargestV()

__device__ bool Combination::isValid(){
    for (int i = 0; i < k; i++){
        if (data[i] < 0 || data[i] > n - 1)
            return false; // value out of range

        for (int j = i+1; j < k; ++j)
            if (data[i] >= data[j])
                return false; // duplicate or not lexicographic
    }

    return true;
}// IsValid()

__device__ Combination Combination::successor(){
    if (data[0] == n - k) return Combination(0, 0);

    Combination ans(n, k);

    int i;
    for (i = 0; i < k; i++)
        ans.data[i] = data[i];

    for (i = k - 1; i > 0 && ans.data[i] == (n - k + i); --i);

    ++ans.data[i];

    for (int j = i; j < k - 1; j++)
        ans.data[j+1] = ans.data[j] + 1;

    return ans;
}// Successor()

// return the mth lexicographic element of combination C(n, k)
__device__ Combination Combination::element(int64 m){
    int ans[4];

    int a = n;
    int b = k;
    int64 x = (choose(n, k) - 1) - m; // x is the "dual" of m

    for (int i = 0; i < k; ++i){
        ans[i] = largestV(a, b, x); // largest value v, where v < a and vCb < x    
        x = x - choose(ans[i], b);
        a = ans[i];
        b = b-1;
    }

    for (int i = 0; i < k; ++i)
        ans[i] = (n-1) - ans[i];

    return Combination(n, k, ans);
}// Element()

__device__ int* Combination::getArray(){
    return data;
} // getArray()
