#include "vml_test.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define FP_TYPE_NUM 4
//32MB
#define FLUSHCACHE_SIZE 32*1024*1024

static eps_t threshold[FP_TYPE_NUM]={{ 1e-04, 1e-05 },
		     { 1e-13, 1e-14 },   // for d
                     { 1e-04, 1e-05 },   // for c
                     { 1e-13, 1e-14 }};

double getRealTime();

#define VML_TEST_LOG printf

void * vml_test_memory_alloc(size_t size)
{
  void * ptr=NULL;
  ptr=malloc(size);
  if(ptr==NULL){
    CTEST_ERR("memory alloc failed!\n");
    exit(0);
  }
  return ptr;
}

void vml_test_memory_free(void * ptr)
{
  free(ptr);
}

void print_help()
{
  printf("-d <start> <end> <step>\n");
  printf("   input data size.\n");
  printf("-s test_suit\n");
  printf("   testsuit name\n");
}

static input_arg_t input_args;

input_arg_t * get_input_arg()
{
  return &input_args;
}

void init_input_arg(int argc, char *argv[])
{
  input_args.start=100;
  input_args.end=200;
  input_args.step=10;
}

void init_test_parameter(perf_arg_t ** p, int iscomplex, int isdouble)
{
  perf_arg_t * parameter=NULL;

  int float_type_id=0;

  parameter=(perf_arg_t*)vml_test_memory_alloc(sizeof(perf_arg_t));

  memset(parameter,0,sizeof(perf_arg_t));

  *p = parameter;

  float_type_id=(iscomplex<<1)|(isdouble);

  if(float_type_id<0 || float_type_id>=FP_TYPE_NUM) {
    CTEST_ERR("Init data error!\n");
  }

  parameter->eps=&threshold[float_type_id];

  parameter->fp_type=float_type_id;
  parameter->element_size=(isdouble==0)?sizeof(float):sizeof(double);
  parameter->compose_size=(iscomplex==0)?1:2;

  parameter->start=input_args.start;
  parameter->end=input_args.end;
  parameter->step=input_args.step;

  parameter->a=vml_test_memory_alloc(parameter->compose_size * parameter->element_size * 
				     parameter->end);

  parameter->y=vml_test_memory_alloc(parameter->compose_size * parameter->element_size * 
				     parameter->end);

  parameter->b=vml_test_memory_alloc(parameter->compose_size * parameter->element_size * 
				     parameter->end);

  parameter->ref_a=vml_test_memory_alloc(parameter->compose_size * parameter->element_size * 
				     parameter->end);

  parameter->ref_y=vml_test_memory_alloc(parameter->compose_size * parameter->element_size * 
				     parameter->end);

  parameter->ref_b=vml_test_memory_alloc(parameter->compose_size * parameter->element_size * 
				     parameter->end);


  parameter->flushcache=vml_test_memory_alloc(FLUSHCACHE_SIZE); 
}

void free_test_parameter(perf_arg_t ** p)
{
  perf_arg_t * parameter=*p;
  if(*p == NULL) return;

  if(parameter->a != NULL){
    vml_test_memory_free(parameter->a);
    parameter->a=NULL;
  }

  if(parameter->y != NULL){
    vml_test_memory_free(parameter->y);
    parameter->y=NULL;
  }

  if(parameter->y != NULL){
    vml_test_memory_free(parameter->y);
    parameter->y=NULL;
  }

  if(parameter->ref_a != NULL){
    vml_test_memory_free(parameter->ref_a);
    parameter->ref_a=NULL;
  }

  if(parameter->ref_y != NULL){
    vml_test_memory_free(parameter->ref_y);
    parameter->ref_y=NULL;
  }

  if(parameter->ref_b != NULL){
    vml_test_memory_free(parameter->ref_b);
    parameter->ref_b=NULL;
  }

  if(parameter->flushcache != NULL){
    vml_test_memory_free(parameter->flushcache);
    parameter->flushcache=NULL;
  }
  vml_test_memory_free(parameter);
}


int check_result(double ref, double test, eps_t* thres)
{
  double tmp;

  tmp=abs(ref-test);

  if(tmp>=thres->warn && tmp<thres->fail) {
    return 1; //warning
  }else if (tmp >= thres->fail) {
    return 2; //error
  }else{
    return 0; //pass
  }
}

int check_vector(VML_INT n, void * ref, void * test, eps_t * thres, int iscomplex, int isdouble)
{
  VML_INT i;
  VML_INT length;
  int result=0, max_result=0;
  length=(iscomplex==0)? n : 2*n;

  if(isdouble==0){
    //float
    float * ref_f=(float*)ref;
    float * test_f=(float*)test;

    for(i=0; i<length; i++){
      result=check_result(ref_f[i], test_f[i], thres);
      if(result>max_result) max_result=result;
    }
  }else{
    //double
    double* ref_d=(double*)ref;
    double * test_d=(double*)test;

    for(i=0; i<length; i++){
      result=check_result(ref_d[i], test_d[i], thres);
      if(result>max_result) max_result=result;
    }
  }
  return max_result;
}

void init_rand_float(VML_INT n, float * a)
{
  VML_INT i=0;
  float e=5.0;
  for(i=0; i<n; i++){
    a[i]=((float)rand()/(float)(RAND_MAX)) * e;
  }
}

void init_rand_double(VML_INT n, double * a)
{
  VML_INT i=0;
  double e=5.0;
  for(i=0; i<n; i++){
    a[i]=((double)rand()/(double)(RAND_MAX)) * e;
  }
}

void init_rand(VML_INT n, void * a, int iscomplex, int isdouble)
{
  VML_INT length;

  length=(iscomplex==0)? n: 2*n;

  if(isdouble==0){
    init_rand_float(length, (float*)a);
  }else{
    init_rand_double(length, (double*)a);
  }
}


void flush_cache(void * flushcache)
{
  if(flushcache!=NULL){
    memset(flushcache, 1, FLUSHCACHE_SIZE);
  }
}

void run_test_ab_y(perf_arg_t * para, char* funcname[], ab_y_func_t test_func[], ab_y_func_t ref_func[],
		   double * flops_per_elem)
{


  VML_INT start=para->start;
  VML_INT end=para->end;
  VML_INT step=para->step;

  double mflops=0.0;
  double time=0.0, start_time, end_time;

  int iscomplex = (para->fp_type & 0x2) >> 1;
  int isdouble = (para->fp_type & 0x1);
  int result=0;
  char * result_str;
  int failed_count=0;

  VML_INT i;

  VML_TEST_LOG("\n");
  VML_TEST_LOG("Func\tN\tMFlops\t\tTime(s)\t\tResult\n");

  init_rand(end, para->a, iscomplex, isdouble);
  init_rand(end, para->b, iscomplex, isdouble);

  memcpy(para->ref_a, para->a, end * para->element_size * para->compose_size);
  memcpy(para->ref_b, para->b, end * para->element_size * para->compose_size);

  for(i=start; i<=end; i+=step) {

    mflops=flops_per_elem[para->fp_type] * i;

    //need to clean cache
    //flush_cache(para->flushcache);

    start_time=getRealTime();
    test_func[para->fp_type](i, para->a, para->b, para->y);
    end_time=getRealTime();
    time=end_time-start_time;

    mflops=mflops/(double)(1000000)/time;

    ref_func[para->fp_type](i, para->ref_a, para->ref_b, para->ref_y);

    //check
    result=check_vector(i, para->ref_y, para->y, para->eps, iscomplex, isdouble);

    if(result==0){
      result_str=STR_PASS;
    }else if(result==1){
      result_str=STR_WARN;
    }else{
      result_str=STR_ERR;
      failed_count++;
    }

    VML_TEST_LOG("%s\t%d\t%lf\t%lf\t%s\n", funcname[para->fp_type], i, mflops, time, result_str);

  }

  if(failed_count>0) CTEST_ERR("Result failed!\n");
}
