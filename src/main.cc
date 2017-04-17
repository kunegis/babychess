#include "main.hh"

#include "verbose.hh"

#include <unistd.h>

#if ! HAVE_PROGRAM_INVOCATION_NAME
const char *program_invocation_name= NULL,
	*program_invocation_short_name= NULL; 
#endif 

#if 1
#include <signal.h>
void main_error(int sig)
{
	fprintf(stderr,
		"[%d] %s:  received %s, pausing\n",
		getpid(),
		program_invocation_name,
		strsignal(sig));
	pause();
	exit(1);
}
#endif /* 1 */ 

int main(int argc, char **argv)
{
#if ! HAVE_PROGRAM_INVOCATION_NAME
	program_invocation_name= argv[0];
	program_invocation_short_name= strrchr(program_invocation_name, '/'+1);
	if (! program_invocation_short_name) 
		program_invocation_short_name= program_invocation_name; 
#endif

#if 1 /* this should be disabled when using the GNOME libraries because
       * they do the same. (also above) */ 
	int sigs[]= {SIGABRT, SIGSEGV, SIGILL, SIGBUS, SIGFPE};
	for (int i= 0;  i < (int) (sizeof(sigs)/sizeof(SIGABRT));  ++i)
		signal(SIGABRT, &main_error);
#endif /* 1 */ 	

	Verbose_Process("entering main()");

	int result= Main(argc, argv);

	Verbose_Process("quitting main()");

	exit (result);
}


/* See etc/MEMORY-LEAK */
#ifdef LOG_MEMORY
/* Prototypes for __malloc_hook, __free_hook */
#include <malloc.h>
#include <stdio.h>

/* Prototypes for our hooks.  */
static void my_init_hook (void);
static void *my_malloc_hook (size_t, const void *);
static void my_free_hook (void*, const void *);
static void *my_realloc_hook (void *, size_t, const void *);


static void *(*old_malloc_hook) (size_t, const void *);
static void (*old_free_hook) (void*, const void *);
static void *(*old_realloc_hook) (void *, size_t,const void *);

static int main_alloc_count= 0;

/* Override initializing hook from the C library. */
void (*__malloc_initialize_hook) (void) = my_init_hook;

static void
my_init_hook (void)
{
	old_malloc_hook = __malloc_hook;
	old_free_hook = __free_hook;
	old_realloc_hook= __realloc_hook;
	__malloc_hook = my_malloc_hook;
	__free_hook = my_free_hook;
	__realloc_hook= my_realloc_hook;
}

static void *
my_malloc_hook (size_t size, const void */*caller*/)
{
	void *result;
	/* Restore all old hooks */
	__malloc_hook = old_malloc_hook;
	__free_hook = old_free_hook;
	__realloc_hook= old_realloc_hook; 
	/* Call recursively */
	result = malloc (size);
	/* Save underlying hooks */
	old_malloc_hook = __malloc_hook;
	old_free_hook = __free_hook;
	old_realloc_hook= __realloc_hook; 
	/* printf might call malloc, so protect it too. */
	assert (result); 
	if (result)  ++main_alloc_count; 
	static int id= 0;
	/* Change -1 in the next line to the ID of a memory chunk that is
	 * never released.  */
	if (id == -1)
	{
		int a= 3;
		a= a;
	}
	fprintf (stderr, "m%d,%p\n", id++, result);
	/* Restore our own hooks */
	__malloc_hook = my_malloc_hook;
	__free_hook = my_free_hook;
	__realloc_hook= my_realloc_hook; 
	return result;
}

static void *
my_realloc_hook (void *p, size_t size, const void */*caller*/)
{
	void *result;
	/* Restore all old hooks */
	__malloc_hook = old_malloc_hook;
	__free_hook = old_free_hook;
	__realloc_hook= old_realloc_hook; 
	/* Call recursively */
	result = realloc (p, size);
	/* Save underlying hooks */
	old_malloc_hook = __malloc_hook;
	old_free_hook = __free_hook;
	old_realloc_hook= __realloc_hook; 
	/* printf might call malloc, so protect it too. */
	assert (p);
	assert (result);
	if (p)  --main_alloc_count;
	if (result)  ++main_alloc_count; 
	assert (main_alloc_count >= 0);
	fprintf(stderr, "r%p,%p\n", result, p);
	/* Restore our own hooks */
	__malloc_hook = my_malloc_hook;
	__free_hook = my_free_hook;
	__realloc_hook= my_realloc_hook; 
	return result;
}

static void 
my_free_hook (void *ptr, const void */*caller*/)
{
	/* Restore all old hooks */
	__malloc_hook = old_malloc_hook;
	__free_hook = old_free_hook;
	__realloc_hook= old_realloc_hook; 
	/* Call recursively */
	free (ptr);
	/* Save underlying hooks */
	old_malloc_hook = __malloc_hook;
	old_free_hook = __free_hook;
	old_realloc_hook= __realloc_hook; 
	/* printf might call free, so protect it too. */
	if (ptr)  --main_alloc_count;
	assert (main_alloc_count >= 0);
	if (ptr)
		fprintf(stderr, "f%p\n", ptr);
	/* Restore our own hooks */
	__malloc_hook = my_malloc_hook;
	__free_hook = my_free_hook;
	__realloc_hook= my_realloc_hook; 
}

#endif /* enabled malloc hooks */ 
