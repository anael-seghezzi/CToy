// Can we compile and run C code dynamically from here too ? Really ?
// Yes we can !!

#include <ctoy.h>
#include <libtcc.h>

const char *script =
"float myc_fun(float x, float y)"
"{"
" return x + y;"
"}";

void ctoy_begin(void)
{
   TCCState *s;

   printf("<sample libtcc_hello>\n");

   s = tcc_new();
   tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
   tcc_add_library_path(s, ".");

   if (tcc_compile_string(s, script) != -1) {
      if (tcc_relocate(s, TCC_RELOCATE_AUTO) >= 0) {

         float (*myc_fun)(float, float) = (float (*)(float, float))tcc_get_symbol(s, "myc_fun");
         float x = myc_fun(0.1, 0.2);
         printf("0.1 + 0.2 = %f\n", x);
      }
   }

   tcc_delete(s);
}

void ctoy_main_loop(void)
{
   ctoy_sleep(0, 1000000);
}

void ctoy_end(void)
{}
