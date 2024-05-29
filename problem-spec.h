/**
 * @文件名：problem-spec.h
 * @描述：问题的具体描述(problem specification)
*/
#ifndef H_PROBLEM_SPEC_H
#define H_PROBLEM_SPEC_H
struct problem_spec{
    double a;               // x=a(左端点)
    double b;               // x=b(右端点)
    double (*ic)(double x); // 初始条件
    double (*bcL)(double t);// 左边界条件
    double (*bcR)(double t);// 右边界条件
    double (*u_exact)(double x, double t);// 精确解
};
#endif
