#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "problem-spec.h"
#include "myarray.h"

/**
 * Heat1: 一维(具有精确解)热传导方程
*/
// 精确解
static double heat1_exact(double x, double t) {
    double Pi = 4 * atan(1.0);
    return exp(-Pi * Pi/4 * t) * cos(Pi/2 * x);
}
// 初值条件
static double heat1_ic(double x) {
    return heat1_exact(x, 0);
}
// 左边界条件
static double heat1_bcL(double t) {
    return heat1_exact(-1, t);
}
// 右边界条件
static double heat1_bcR(double t) {
    return heat1_exact(1, t);
}
// 问题1
struct problem_spec *heat1(void){
    static struct problem_spec spec = {  // 初始化
        .a = -1.0,
        .b = 1.0,
        .ic = heat1_ic,
        .bcL = heat1_bcL,
        .bcR = heat1_bcR,
        .u_exact = heat1_exact,
    };
    printf("[一维热传导方程]问题1：\n");
    return &spec;
}

/**
 * Heat3:矩形凸块(rectangular bump)的热传导方程
 */

/**
 *  初始条件是矩形凸块函数u₀
 *              ┌
 *      u₀(x) = | 1, |x|<0.4,
 *              | 0, 其他
 *              └
 *
 * */
// static double heat3_ic(double x){
//     return fabs(x) < 0.4 ? 1 : 0;
// }

