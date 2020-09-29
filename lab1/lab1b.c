#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>


int option_index;
int c;
int num_files = 0;
int num_files_cap = 10;
int* file_descriptor_table; 
int exit_status = 0;
int verbose_flag = 0;
int file_flag = 0;
int N;
char *null_ptr = NULL;
int num_commands = 0;
int exit_code;
pid_t child_process_id;

int max_exit_code = 0;

void catch(int N){
  fprintf(stderr, "%d %s", N, "caught \n");
  exit(N);
}



void realloc_file_descriptor_array(int n){

  if (n >= num_files_cap){
    num_files_cap += 20;
    file_descriptor_table = (int*)realloc(file_descriptor_table, num_files_cap * sizeof(int));
  }
  return;

}

struct child_process_info{
  pid_t child_pid;
  char** command;
  int num_args;
};


struct child_process_info* child_process_list;


int main(int argc, char *argv[]){

  child_process_list = (struct child_process_info*) malloc(sizeof(struct child_process_info)*(num_commands +1));


  file_descriptor_table = (int *) malloc( num_files_cap * sizeof(int));

  static struct option long_options[] = {

    //file openers
    {"rdonly", required_argument, 0, 'r'},
    {"wronly", required_argument, 0, 'w'},
    {"rdwr", required_argument, 0, 'd'},
    {"pipe", no_argument, 0, 'p'},

    //subcommands
    {"command", required_argument, 0, 'c'},
    {"wait", no_argument, 0, 'h'}, //h for hol up..

    //File Flags
    {"append", no_argument, 0, '1'},
    {"cloexec", no_argument, 0, '2'},
    {"creat", no_argument, 0, '3'},
    {"directory", no_argument, 0, '4'},
    {"dsync", no_argument, 0, '5'},
    {"excl", no_argument, 0, '6'},
    {"nofollow", no_argument, 0, '7'},
    {"nonblock", no_argument, 0, '8'},
    {"rsync", no_argument, 0, '9'},
    {"sync", no_argument, 0, '!'},
    {"trunc", no_argument, 0, '@'},

    //misc. commands
    {"verbose", no_argument, 0, 'v'},
    {"close", required_argument, 0, 'l'},
    {"abort", no_argument, 0, 'a'},
    {"ignore", required_argument, 0, 'i'},
    {"catch", required_argument, 0, 'g'}, // for gotcha
    {"default", required_argument, 0, 'f'},
    {"pause", no_argument, 0, 'u'},

    {0,0,0,0}

  };

  while( (c = getopt_long(argc, argv, "", long_options, &option_index)) != -1){

    switch(c){

    case 'h':
      if(verbose_flag == 1){
	printf("--wait \n");
	fflush(stdout);
      }
      while ((child_process_id = waitpid(-1, &exit_code, 0)) > 1){
	if (child_process_id < 0){

	  fprintf(stderr, "error getting exit status of child process. \n");
	  fflush(stdout);
	  break;
	}
	else if (WIFEXITED(exit_code)) {

	  printf("exit %d ", WEXITSTATUS(exit_code));
	  fflush(stdout);
	  for (int i = 0; i < num_commands; i++){

	    if (child_process_list[i].child_pid == child_process_id){

	      for (int k = 0; k < child_process_list[i].num_args; k++){
		printf("%s ", child_process_list[i].command[k]);
		fflush(stdout);
	      }
	    }

	  }

	  printf("\n");
	  fflush(stdout);
	  if ( WEXITSTATUS(exit_code) > max_exit_code){
	    max_exit_code = WEXITSTATUS(exit_code);
	    exit_status = max_exit_code;
	  }
	}
	
      
        else if (WIFSIGNALED(exit_code)) {

	  printf("signal %d ", WTERMSIG(exit_code));
	  fflush(stdout);
	  for (int i = 0; i < num_commands; i++){

	    if (child_process_list[i].child_pid == child_process_id){

	      for (int k = 0; k < child_process_list[i].num_args; k++){
		printf("%s ", child_process_list[i].command[k]);
		fflush(stdout);
	      }
	    }

	  }

	printf("\n");
	fflush(stdout);

	if ((128 +  WTERMSIG(exit_code)) > max_exit_code){
	  max_exit_code = 128 + WTERMSIG(exit_code);
	  exit_status = max_exit_code;
	}
      }


      }

      break;




    case 'u':
      if (verbose_flag ==1)
	{
	  printf("--pause \n");
	  fflush(stdout);
	}
      pause();
      break;
    case 'f':

      if (verbose_flag ==1)
	{
	  printf("--default %s \n",optarg);
	  fflush(stdout);
	}
      N = atoi(optarg);

      if( N < 0 ){
	fprintf(stderr, "error with argument for --default \n");
	exit_status = 1;
	break;
      }
      signal(N,SIG_DFL);
      
      break;


    case 'g':
      N = atoi(optarg);
      if (verbose_flag ==1)
        {
          printf("--catch %s \n",optarg);
	  fflush(stdout);
        }
      if( N < 0 ){
	fprintf(stderr, "error with argument for --catch \n");
	exit_status = 1;
	break;
      }
      signal(N,catch);
      
      break;

    case 'i':
      if (verbose_flag ==1)
	{
	  printf("--ignore \n");
	  fflush(stdout);
	}

      N = atoi(optarg);

      if( N < 0 ){
	fprintf(stderr, "error with argument for --ignore \n");
	exit_status = 1;
	break;
      }
      signal(N,SIG_IGN);
      
      break;

    case 'a':

      if (verbose_flag ==1)
        {
          fprintf(stdout,"--abort \n");
	  fflush(stdout);
	}

      *null_ptr = 'k';
      exit_status = 1;
      break;

      //close N
    case 'l':	
	
	
      if (verbose_flag ==1)
	{
	  printf("--pipe %s \n",optarg);
	  fflush(stdout);
	}
      N = atoi(optarg);

      if( N < 0 ){
	fprintf(stderr, "error with argument for --close \n");
	exit_status = 1;
      }

      else if(file_descriptor_table[N] == -1 || N >= num_files ){
        fprintf(stderr, "error closing file");
        exit_status = 1;

      }
      else {
        if (close(file_descriptor_table[N]) == -1){
	  fprintf(stderr, "error closing file");
	}
        file_descriptor_table[N] = -1;
      }

      break;

     


      /****** declare file opening flags ******/
    case'1':
      if(verbose_flag == 1){
        printf("--append\n");
        fflush(stdout);
      }
      file_flag |= O_APPEND;
      break; 

    case'2':
      if(verbose_flag == 1){
        printf("--cloexec\n");
        fflush(stdout);
      }
      file_flag |= O_CLOEXEC;
      break;

    case'3':
      if(verbose_flag == 1){
        printf("--creat\n");
        fflush(stdout);
      }
      file_flag |= O_CREAT;
      break;

    case'4':
      if(verbose_flag == 1){
        printf("--directory\n");
        fflush(stdout);
      }
      file_flag |= O_DIRECTORY;
      break;

    case'5':
      if(verbose_flag == 1){
        printf("--dsync\n");
        fflush(stdout);
      }
      file_flag |= O_DSYNC;
      break;

    case'6':
      if(verbose_flag == 1){
        printf("--excl\n");
        fflush(stdout);
      }
     
      file_flag |= O_EXCL;
      break;

    case'7':
      if(verbose_flag == 1){
        printf("--nofollow\n");
        fflush(stdout);
      }
      file_flag |= O_NOFOLLOW;
      break;

    case'8':
      if(verbose_flag == 1){
        printf("--nonblock\n");
        fflush(stdout);
      }
      file_flag |= O_NONBLOCK;
      break;

    case'9':
      //undecclared???
      if(verbose_flag == 1){
        printf("--rsync\n");
        fflush(stdout);
      }
      file_flag |= O_RSYNC;
      break;

    case'!':
      if(verbose_flag == 1){
        printf("--sync\n");
        fflush(stdout);
      }
      file_flag |= O_SYNC;
      break;

    case'@':
      if(verbose_flag == 1){
        printf("--trunc\n");
        fflush(stdout);
      }
      file_flag |= O_TRUNC;
      break;
      /****** ******** ******/

    

    case 'p':
      if (verbose_flag ==1)
	{
	  printf("--pipe \n");
	  fflush(stdout);
	}
      int pipe_file_descriptor[2];
      if (pipe(pipe_file_descriptor) < 0){
	fprintf(stderr, "error opening pipe");
	exit_status = -1;
	break;
      }

      file_descriptor_table[num_files] = pipe_file_descriptor[0];
      num_files += 1;
      if (file_descriptor_table[num_files] == -1 ){
	fprintf(stderr, "error opening file: %s \n", optarg);
	exit_status = 1;
      }
      realloc_file_descriptor_array(num_files);

      file_descriptor_table[num_files] = pipe_file_descriptor[1];
      num_files+= 1;

      if (file_descriptor_table[num_files] == -1 ){
	fprintf(stderr, "error opening file: %s \n", optarg);
	exit_status = 1;
      }
      realloc_file_descriptor_array(num_files);


      break;



    case 'v':
      verbose_flag = 1;
      break;

    case 'r':
      //printf("rdonly \n");
      //printf("opening file: %s \n",optarg);
      if (verbose_flag ==1)
	{
	  printf("--rdonly %s \n",optarg);
	  fflush(stdout);
	}

      file_flag |= O_RDONLY;
      file_descriptor_table[num_files] = open(optarg,file_flag,0666);

      if (file_descriptor_table[num_files] == -1 ){
	fprintf(stderr, "error opening file: %s \n", optarg);
	exit_status = 1;
      }

      num_files += 1;

      realloc_file_descriptor_array(num_files);
      file_flag = 0;
      break;

    case 'w':

      if (verbose_flag ==1)
	{
	  printf("--wronly %s \n",optarg);
	  fflush(stdout);
	}

      file_flag |= O_WRONLY;

      file_descriptor_table[num_files] = open(optarg,file_flag,0666);

      if (file_descriptor_table[num_files] == -1 ){
	fprintf(stderr, "error opening file: %s \n", optarg);
	fflush(stdout);
	exit_status = 1;
      }
 
      num_files += 1;

      realloc_file_descriptor_array(num_files);
      file_flag = 0;
      break;

      //rdrw
    case 'd':

      if (verbose_flag ==1)
	{
	  printf("--rdwr %s \n",optarg);
	  fflush(stdout);
	}

      file_flag |= O_RDWR;


      file_descriptor_table[num_files] = open(optarg,file_flag,0666);
      
      if (file_descriptor_table[num_files] == -1 ){
	fprintf(stderr, "error opening file: %s \n", optarg);
	exit_status = 1;
      }

      num_files += 1;

      realloc_file_descriptor_array(num_files);
      file_flag = 0;
      break;


    case 'c':

      optind -= 1;



      char **command;
      int files[3];
      int num_args_cap = 10;
      int num_args = 0;
      int pid;
      command = malloc(num_args_cap*sizeof(char*));

      // get 3 file descriptors, files[3]
      for (int i=0; i<3; i++){

	if( (files[i] = atoi( argv[optind]) ) <= 0 ){
	  //atoi sucks and can't tell diffference between 0 and an error converting
	  //this is to get around that
	  if (*argv[optind] != '0'){
	    fprintf(stderr, "--command file descriptors arguements must be nonnegative ints \n");
	    exit_status = 1;
	    break;
	  }
	}
	
	if (files[i] >= num_files || file_descriptor_table[files[i]] == -1){
	  fprintf(stderr, "file descriptor does not exist \n");
	  exit_status = 1;
	  break;
	}
	

	optind++;
      }


      //get command to run
      





                  
      while( optind < argc  && !(argv[optind][0] == '-' && argv[optind][1] == '-')){

	command[num_args] = argv[optind];

	if (num_args > num_args_cap){
	  num_args_cap += 10;
	  command = realloc(command, num_args_cap*sizeof(char*));
	}
	
	optind++;
	num_args++;  

      }
      
      if(num_args == 0){
	fprintf(stderr, "no command given  \n");
	fflush(stdout);
	exit_status = 1;
      }





      

      if(verbose_flag == 1){

	printf("%s ", "--command");
	for(int i = 0; i < 3; i++){
	  printf("%d ", files[i]);
	}
	for(int i = 0; i < num_args; i++){
	  printf("%s ", command[i]);
	}
	printf("\n");
	fflush(stdout);

      }


      pid = fork();
      if (pid == 0){

	int duperror = 0;

	if(dup2(file_descriptor_table[files[0]], 0) < 0 )
	  duperror = 1;
	if(dup2(file_descriptor_table[files[1]], 1) < 0 )
	  duperror = 1;
	if(dup2(file_descriptor_table[files[2]], 2) < 0 )
	  duperror = 1;

	if (duperror == 1) {

	  fprintf(stderr, "error using dup on specified file descriptors \n");
	  exit_status = 1;
	  break;
	}
	

	int exec_status;
	

	for(int i = 0; i < num_files; i++){
          close(file_descriptor_table[i]);
        }

	exec_status = execvp(command[0], command);
	
	if (exec_status < 0 ){


	  exit_status = 1;
	  break;
	}





	free(command);

	
      }

      else if( pid > 0){


	
	child_process_list[num_commands].child_pid = pid;

	child_process_list[num_commands].command = command;

	child_process_list[num_commands].num_args = num_args;
	/*

	  printf("HELLO \n");

	  for(int i = 0; i < num_args; i++){
	  printf("%s ", child_process_list[num_commands].command[i]);
	  }
	  printf("\n");

	*/

	num_commands += 1;
	child_process_list = (struct child_process_info*) realloc(child_process_list,sizeof(struct child_process_info)*(num_commands +1));

	


      }
 
      break;

    default:
      if (max_exit_code > 1){
	exit(max_exit_code);
      }
      exit(1);
      break;

    }
  }

  for (int i = 0; i < num_files; i++){
    if (file_descriptor_table[i] != -1)
      close(file_descriptor_table[i]);
  }

  if (max_exit_code > 1){
    exit(max_exit_code);
  }

  exit(exit_status);

  return 0;

}
