#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include "ap_global1.h"

#include "num.h"
#include "itv.h"
#include "t1p_internal.h"
#include "t1p_fun.h"
#include "t1p_constructor.h"
#include "t1p_representation.h"
#include "t1p_join_alt.h"

void test_equation_1 (FILE* stream, ap_manager_t* man);


/* construct an equation and print it */
void test_equation_1 (FILE* stream, ap_manager_t* man)
{
  t1p_internal_t* pr = man->internal;

  /* variables and noise symbols */
  ap_dim_t x0 = (ap_dim_t) 0;
  ap_dim_t x1 = (ap_dim_t) 1;
  ap_dim_t x2 = (ap_dim_t) 2;

  t1p_nsym_t* peps1 = t1p_nsym_add(pr, IN);
  t1p_nsym_t* peps2 = t1p_nsym_add(pr, IN);
  t1p_nsym_t* peps3 = t1p_nsym_add(pr, IN);
  
  ja_eq_t* equation1= new_equation ();
  ja_eq_t* equation2= new_equation ();
  ja_eq_set_t* eqs = new_equation_set ();

  /* constants */
  itv_t zero,un,moinsun;
  itv_init(zero);
  itv_set_int(zero,0);
  itv_init(un);
  itv_set_int(un,1);
  itv_init(moinsun);
  itv_set_int(moinsun,-1);

  /* equation1 x0=x1+epsilon1 -epsilon2*/
  equation1->pdim= &x0;
  itv_set_int(equation1->c,0);
  add_equation_term_va (equation1, un, &x1);
  add_equation_term_ns (equation1, un, peps1);
  add_equation_term_ns (equation1, moinsun, peps2);

  /* print it */
  print_equation(stream,equation1);

  /* add to eqs, then renintialize */
  add_equation (eqs,equation1);
  
 /* equation2 x2=1-x0+x1+epsilon1 -epsilon2 +epsilon3 */
  equation2->pdim= &x2;
  itv_set_int(equation2->c,1);
  add_equation_term_va (equation2, moinsun, &x0);
  add_equation_term_va (equation2, un, &x1);
  add_equation_term_ns (equation2, un, peps1);
  add_equation_term_ns (equation2, moinsun, peps2);
  add_equation_term_ns (equation2, un, peps3);

  /* print it */
  print_equation(stream,equation2);

  /* add to eqs, then renintialize */
  add_equation (eqs,equation2);

  fprintf(stream,"**********\n**********\n**********\n");
  print_equation_set(stream,eqs);

  free_equation (equation1);
  free_equation (equation2);
  free_equation_set(eqs);
}


void test_rebuild_1 (FILE* stream, ap_manager_t* man)
{
  t1p_internal_t* pr = man->internal;

  /* variables and noise symbols */
  ap_dim_t x0 = (ap_dim_t) 0;
  ap_dim_t x1 = (ap_dim_t) 1;
  ap_dim_t x2 = (ap_dim_t) 2;

  t1p_nsym_t* peps0 = t1p_nsym_add(pr, IN);
  t1p_nsym_t* peps1 = t1p_nsym_add(pr, IN);
  t1p_nsym_t* peta0 = t1p_nsym_add(pr, UN);
  
  ja_eq_t* equation1= new_equation ();
  ja_eq_t* equation2= new_equation ();
  ja_eq_set_t* eqs = new_equation_set ();

  /* constants */
  itv_t zero,un,moinsun,trois;
  itv_init(zero);
  itv_set_int(zero,0);
  itv_init(un);
  itv_set_int(un,1);
  itv_init(moinsun);
  itv_set_int(moinsun,-1);
  itv_init(trois);
  itv_set_int(trois,3);

  /* equation1 x0=1+x1+epsilon0 -epsilon1*/
  equation1->pdim= &x0;
  itv_set_int(equation1->c,1);
  add_equation_term_va (equation1, un, &x1);
  add_equation_term_ns (equation1, trois, peps0);
  add_equation_term_ns (equation1, moinsun, peps1);

  /* print it */
  //  print_equation(stream,equation1);

  /* add to eqs, then renintialize */
  add_equation (eqs,equation1);


  /* creation of the abstract value x1 = 3+ eps1 + eta0 */
  t1p_aff_t* afexpr = t1p_aff_alloc_init(pr);
  itv_set(afexpr->c, trois);
  t1p_aff_nsym_add(pr,afexpr,un,peps1);
  t1p_aff_nsym_add(pr,afexpr,un,peta0);
  itv_set_int2(afexpr->itv,1,5);
  
  t1p_t* a = t1p_alloc(man,2,0) ;
  //  printf("%p\n",a->paf[1]);
  t1p_aff_t* afexpr2 = t1p_aff_alloc_init(pr);
  afexpr2=t1p_aff_copy(pr,pr->top);
  a->paf[0] = afexpr2;
  a->paf[0]->pby++;
  itv_set_top(a->box[0]);
  a->paf[1] =afexpr;
  a->paf[1]->pby++;
  itv_set(a->box[1],afexpr->itv);




  fprintf(stream,"**********\n**********\n**********\n");
  fprintf(stream,"equations:\n");
  print_equation_set(stream,eqs);


  //t1p_aff_fprint(pr,stream,a->paf[0]);
  //fprintf(stream,"\n");
  //t1p_aff_fprint(pr,stream,afexpr);
  //fprintf(stream,"\n");
  fprintf(stream,"before operation:\n");
  t1p_fdump(stream,man,a);

  fprintf(stream,"**********\n**********\n**********\n");


  //rebuild

  rebuild_abstract_value(man,false,a,eqs);

  fprintf(stream,"after operation:\n");
  t1p_fdump(stream,man,a);


  //a->paf[0] =NULL;
  //a->paf[0]->pby--;
  //a->paf[1] =NULL;
  //a->paf[1]->pby--;
  free(a);
  free(afexpr2);
  free(afexpr);  
  free_equation (equation1);
  free_equation_set(eqs);

}


void test_generate_1(FILE* stream, ap_manager_t* man)
{
  t1p_internal_t* pr = man->internal;
  ja_eq_set_t* eqs=new_equation_set();

  /* variables and noise symbols */
  ap_dim_t x0 = (ap_dim_t) 0;
  ap_dim_t x1 = (ap_dim_t) 1;
  ap_dim_t x2 = (ap_dim_t) 2;

  t1p_nsym_t* peps0 = t1p_nsym_add(pr, IN);
  t1p_nsym_t* peps1 = t1p_nsym_add(pr, IN);
  t1p_nsym_t* peta0 = t1p_nsym_add(pr, UN);

  /* first abstract value */
  t1p_t* a1 = t1p_alloc(man,2,0) ;
  t1p_aff_t* afexpr10 = t1p_aff_alloc_init(pr);
  t1p_aff_t* afexpr11 = t1p_aff_alloc_init(pr);

 /* second abstract value */
  t1p_t* a2 = t1p_alloc(man,2,0) ;
  t1p_aff_t* afexpr20 = t1p_aff_alloc_init(pr);
  t1p_aff_t* afexpr21 = t1p_aff_alloc_init(pr);

  /* constants */
  itv_t zero,un,moinsun,deux,quatre;
  itv_init(zero);
  itv_set_int(zero,0);
  itv_init(un);
  itv_set_int(un,1);
  itv_init(moinsun);
  itv_set_int(moinsun,-1);
  itv_init(deux);
  itv_set_int(deux,2);
  itv_init(quatre);
  itv_set_int(quatre,4);

  /* build the first abstract value */
  itv_set(afexpr10->c, deux);
  t1p_aff_nsym_add(pr,afexpr10,un,peps0);
  itv_set_int2(afexpr10->itv,1,3);
  itv_set(afexpr11->c, deux);
  t1p_aff_nsym_add(pr,afexpr11,un,peps1);
  itv_set_int2(afexpr11->itv,1,3);
  a1->paf[0] = afexpr10;
  a1->paf[0]->pby++;
  itv_set(a1->box[0],afexpr10->itv);
  a1->paf[1] =afexpr11;
  a1->paf[1]->pby++;
  itv_set(a1->box[1],afexpr11->itv);

 /* build the second abstract value */
  itv_set(afexpr20->c,quatre);
  t1p_aff_nsym_add(pr,afexpr20,un,peps0);
  itv_set_int2(afexpr20->itv,3,5);
  itv_set(afexpr21->c,quatre);
  t1p_aff_nsym_add(pr,afexpr21,un,peps1);
  itv_set_int2(afexpr21->itv,3,5);
  a2->paf[0] = afexpr20;
  a2->paf[0]->pby++;
  itv_set(a2->box[0],afexpr20->itv);
  a2->paf[1] =afexpr21;
  a2->paf[1]->pby++;
  itv_set(a2->box[1],afexpr21->itv);

  fprintf(stream,"\n a1:\n");
  t1p_fdump(stream,man,a1);
  fprintf(stream,"\n a2:\n");
  t1p_fdump(stream,man,a2);
  fprintf(stream,"**********\n**********\n**********\n");

  fprintf(stream,"a1->size %d\n",a1->size);
  fprintf(stream,"a2->size %d \n",a2->size);

  int i;
  for (i=0 ; i<(int)a1->dims ; i++)
    {
      fprintf(stream,"a1[%d]->end= ",i);
      t1p_aaterm_fprint(pr,stream,a1->paf[i]->end);
      fprintf(stream,"\n");

      fprintf(stream,"a2[%d]->end= ",i);
      t1p_aaterm_fprint(pr,stream,a2->paf[i]->end);
      fprintf(stream,"\n");
    }
  /* extract the equations */
  eqs= abstract_value_to_eq_set (pr,a1,a2);
  print_equation_set(stream,eqs);


  /* free */
  free(a1);
  free(afexpr10);
  free(afexpr11);  
  free(a2);
  free(afexpr20);
  free(afexpr21); 
  free_equation_set(eqs); 
}


int main (void)
{
  ap_manager_t* t1p = t1p_manager_alloc();
  FILE* stream = fopen("toto.log","w");

  //test_equation_1(stream,t1p);
  test_rebuild_1(stream,t1p);
  //test_generate_1(stream,t1p);
  fclose(stream);
  ap_manager_free(t1p);

  return 0;
}
      