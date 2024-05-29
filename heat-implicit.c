/**
 * @file heat-implicit.c
 * @brief 该文件包含了隐式格式求解热传导方程的函数
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "problem-spec.h"
#include "myarray.h"

// 声明函数(接口)
static void trisolve(int n, double *a, double *d, double *c, double *b, double *x);
static double get_error(struct problem_spec *spec, double *u, int n, double T);
static void plot_curve(FILE *fp, double *u, int n, int steps, int k);
static void heat_implicit(struct problem_spec *spec, double T, int n, int steps, char *gv_filename);
static void show_usage(char *progname);

// 函数实现
/**
 * @name: trisolve
 * @msg: 求解三对角矩阵
 * @param:
 *       n：方阵的维数
 *       a：主对角线下方的对角向量
 *       d：主对角线的对角向量
 *       c：主对角线上方的对角向量
 *       b：常数向量
 *       x：解向量
 * ┌-                    -┐   ┌    ┐ ┌    ┐ 
 * |d₀   c₀               |   | x₀ | | b₀ | 
 * |a₀   d₁   c₁          |   | x₁ | | b₁ |
 * |     a₁   d₂   c₂     |   | x₂ | | b₂ |  
 * |       ⋱    ⋱   ⋱     |   |  ⋮ |=|  ⋮ |  
 * |        aₙ₋₃ dₙ₋₃ cₙ₋₃|   |xₙ₋₂| |bₙ₋₂|
 * |             aₙ₋₂ dₙ₋₁|   |xₙ₋₁| |bₙ₋₁|
 * └-                    -┘ₙₓₙ└    ┘ └    ┘ 
 *          A                   x   =   b
 *
 * 因为方阵A是稀疏矩阵，
 *  所以向量a存储主对角线下方的对角向量 [a₀,a₁⋯,aₙ₋₂]
 *  所以向量d存储主对角线的对角向量     [d₀,d₁⋯,dₙ₋₁]
 *  所以向量c存储主对角线上方的对角向量 [c₀,c₁⋯,cₙ₋₂]
 * 
 * 化简完的结果是
 * ┌-                    -┐   ┌    ┐ ┌     ┐ 
 * |d'₀ c₀                |   | x₀ | | b'₀ | 
 * |    d'₁    c₁         |   | x₁ | | b'₁ |
 * |        d'₂   c₂      |   | x₂ | | b'₂ |  
 * |          ⋱     ⋱     |   |  ⋮ |=|  ⋮  |  
 * |           d'ₙ₋₃ cₙ₋₃ |   |xₙ₋₂| |b'ₙ₋₂|
 * |                 d'ₙ₋₁|   |xₙ₋₁| |b'ₙ₋₁|
 * └-                    -┘ₙₓₙ└    ┘ └     ┘ 
 * @return void
 *    但是传入的是x指针，所以计算结果仍是x
 */ 
static void trisolve(int n, double *a, double *d, double *c, double *b, double *x){
    // 先化简成上三角矩阵
    for (int i = 1; i < n; i++) {
        double m = a[i-1]/d[i-1];
        d[i] -= m*c[i-1];
        b[i] -= m*b[i-1];
    }
    // 再求解x
    x[n-1] = b[n-1]/d[n-1];
    for (int i = n-2; i>=0; i--) {
      x[i] = (b[i] - c[i]*x[i+1])/d[i];
    }
}
/**
 * @name: get_error
 * @msg: 计算误差
 * @param:  
 *      spec：问题的具体描述
 *      u：解向量
 *      n：网格x的划分数
 *      T：时间t的区间
 * @return: 误差
*/
static double get_error(struct problem_spec *spec, double *u, int n, double T){
    double err = 0.0;
    for (int j=0; j < n+2; j++) {
        double x =spec->a + (spec->b - spec->a)/(n+1)*j;
        double diff = fabs(u[j] - spec->u_exact(x, T));
        if (diff > err) {
            err = diff;            
        }
    }
    return err;
}
/**
 * @name: plot_curve
 * @msg: 根据geomview格式绘制曲线
 * @param:
 *      fp：文件指针
 *      u：解向量
 *      n：网格x的划分数
 *      steps：时间t的划分数
 *      k：时间步长
 * @return: void
*/
static void plot_curve(FILE *fp, double *u, int n, int steps, int k){
    for(int j=0; j < n+2; j++){
        fprintf(fp, "%g %g %g\n",
               (double) k/steps, (double)j/(n+1), u[j]);
    }
}
static void heat_implicit(struct problem_spec *spec, double T, int n, int steps, char *gv_filename){
    FILE *fp;
    double *u, *v, *d, *c;
    double dx = (spec->b - spec->a)/(n+1);
    double dt = T/steps;
    double r = dt/(dx*dx);
    if (NULL == (fp = fopen(gv_filename, "w"))) {
        fprintf(stderr, "文件'%s'不能打开写入内容\n", gv_filename);
         return;
    }
    // 开始写 Geomview 脚本
    fprintf(fp, "# 函数%s 开始写入Geomview 脚本\n",__func__);
    fprintf(fp, "{ appearance { +edge }\n");
    fprintf(fp, "MESH %d %d\n", n+2, steps+1);
    printf("%g < x < %g, 0 < t < %g, dx = %g, dt = %g, "
            "r = dt/dx^2 = %g\n",
            spec->a, spec->b, T, dx, dt, r);
    make_vector(u, n+2);
    make_vector(v, n+2);
    make_vector(d, n);
    make_vector(c, n-1);
    for (int j = 0; j < n+2; j++) {
        double x = spec->a + (spec->b - spec->a)/(n+1)*j;
        u[j] = spec->ic(x);
    }
    plot_curve(fp, u, n, steps, 0); 

    for (int j = 0; j < n-1; j++) {
        c[j] = -r;
    }
    for (int k = 1; k <= steps; k++) {
        double *tmp;
        double t = T*k/steps;
        v[0] = spec->bcL(t);
        v[n+1] = spec->bcR(t);
        u[1] += r*v[0];
        u[n] += r*v[n+1];
        for (int i = 0; i < n; i++) {
            d[i] = 1 + 2*r;
        }
        trisolve(n, c, d, c, u+1, v+1);
        tmp = v;
        v = u;
        u = tmp;
        plot_curve(fp, u, n, steps, k);
    }
    fprintf(fp, "}\n");
    fclose(fp);
    // Geomview脚本写完
    printf("geomview 脚本写入 %s文件中\n", gv_filename);
    if(spec->u_exact != NULL){
        double err = get_error(spec, u, n, T);
        printf("%g时刻的最大误差为%g\n", T, err);
    }
    free_vector(u);
    free_vector(v);
    free_vector(d);
    free_vector(c);
    putchar('\n');
}

/*
 * @name: show_usage
 * @msg: 打印程序使用方法
 * @param: progname 程序名
 */
static void show_usage(char *progname){
    // 打印程序使用方法
    // 检查argc,如果参数个数不对，打印程序使用方法，并退出
        printf("Usage: %s T n s \n", progname);
        printf("  T：时间t的区间 0 ≤ t ≤ T\n");
        printf("  n：网格x的划分 a=x[0], x[1], ..., x[n], x[n+1]=b\n");
        printf("  s：时间t的划分 0=t[0], t[1], ..., t[s]=T\n");
        exit(1);
}

int main(int argc, char *argv[])
{
    struct problem_spec *heat1(void);
    // struct problem_spec *heat2(void);
    // struct problem_spec *heat3(void);
    // struct problem_spec *heat4(void);
    char *endptr;
    double T;
    int n, steps;
    // 判断接收的参数个数是否正确
    if (4 != argc){
        show_usage(argv[0]);
        return EXIT_FAILURE;
    }
    // 判断接收的参数是否正确
    T = strtod(argv[1], &endptr); // 字符串转double
    if(*endptr != '\0' || T <= 0.0){
        show_usage(argv[0]);
        return EXIT_FAILURE;
    }
    n = strtol(argv[2], &endptr, 10);// 字符串转long int
    if(*endptr != '\0' || n < 1){
        show_usage(argv[0]);
        return EXIT_FAILURE;
    }
    steps = strtol(argv[3], &endptr, 10);
    if(*endptr != '\0' || steps <0){
        show_usage(argv[0]);
        return EXIT_FAILURE;
    }
    heat_implicit(heat1(), T, n, steps, "im1.gv");
    // heat_implicit(heat2(), T, n, steps, "im2.gv");
    // heat_implicit(heat3(), T, n, steps, "im3.gv");
    // heat_implicit(heat4(), T, n, steps, "im4.gv");

    return EXIT_SUCCESS;
}
