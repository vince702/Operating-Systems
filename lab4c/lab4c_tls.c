
/*NAME: VINCENT CHI
*EMAIL: vincentchi9702@gmail.com
*ID: 304576879
*/  



#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <mraa.h>
#include <mraa/gpio.h>
#include <mraa/aio.h>
#include <math.h>
#include "fcntl.h"

#include <poll.h>
#include <sys/time.h>


int opt = 0;
int stop_flag = 0;
int log_flag = 0;
int temp_metric = 1; //0 for Celcius, 1 for Farenheit
char* log_file;
int log_fd;
struct pollfd pfd;
int id_flag = 0;
int host_flag = 0;

struct tm* print_current_time;
time_t current;
time_t next;
int period = 1;



//mraa inputs
mraa_aio_context sensor;
mraa_gpio_context button;


#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>




int socket_fd = 0;
struct sockaddr_in address;
struct hostent *server;
char* hostname = NULL;
int id = 0;
int port_num = 0;

SSL_CTX *sslctx;
SSL *ssl;


void SSL_init()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    sslctx = SSL_CTX_new( SSLv23_client_method() );

    if(sslctx == NULL){
        fprintf(stderr, "error with SSL_CTX creation \n");
        exit(1);

    }

    ssl = SSL_new(sslctx);


    SSL_set_fd(ssl, socket_fd);

    int connect = SSL_connect(ssl);

    if (connect != 1) {
        fprintf(stderr, "error establishing SSL/TLS session \n");
        exit(2);
    }





}



void tcp_init(){

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if( socket_fd == -1 ){
         fprintf(stderr, "error with getting socket fd \n");
    }

    server = gethostbyname(hostname);

    if ( server == NULL ){

        fprintf(stderr, "error with getting server \n");

    } 

    memset( (char*)&address, 0, sizeof(address));

    address.sin_family = AF_INET;

    memcpy( (char*)&address.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

    address.sin_port = htons(port_num);


    if( connect(socket_fd, (struct sockaddr*)&address, sizeof(address)) ==  -1){

        fprintf(stderr, "error with connection to socket \n");
        exit(2);

    }

}




float convert_temperature(int a){

    float R = 100000.0 * (1023.0/((float)a)-1.0);
    const int B = 4275;               // B value of the thermistor
    const float R0 = 100000.0;            // R0 = 100k
    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;

    if (temp_metric == 0)
    	return temperature;
    else 
    	return (temperature * 9/5) + 32;

}


void print_temp(float temperature){

	print_current_time = localtime(&current);
    char buf[256];
	sprintf(buf, "%.2d:%.2d:%.2d %.1f\n", print_current_time->tm_hour, print_current_time->tm_min, print_current_time->tm_sec, temperature);
    if(SSL_write(ssl, buf, strlen(buf)) == -1){
        fprintf(stderr, "error with SSL_write \n");
        exit(2);
    }
	if (log_flag == 1){
        	dprintf(log_fd,"%.2d:%.2d:%.2d %.1f\n", print_current_time->tm_hour, print_current_time->tm_min, print_current_time->tm_sec, temperature);
    	}

}




int main(int argc, char** argv){

    static struct option options [] = {
        {"period", required_argument, 0, 'p'},
        {"scale", required_argument, 0, 's'},
        {"log", required_argument, 0, 'l'},
        {"id", required_argument, 0, 'i'},
        {"host", required_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

 	while( (opt = getopt_long(argc, argv, "", options, NULL)) != -1 ){
        switch(opt){
            case 'p':
             	period = (int)atoi(optarg);
                if(period <= 0){
                    
						//error
                	fprintf(stderr, "error with input for period \n");
			exit(1);
                }
                break;
            case 's':
		      switch(*optarg){
            	   case 'C':	
			temp_metric = 0;
			break;
		   case 'F':
            		temp_metric = 1;
            	   	break;
		   default:

            		//error
            		fprintf(stderr, "error with input for scale, arguments must be either C or F \n");
        			exit(1);
        			break;
            	
		      }
		      break;


            case 'l':
            	log_flag = 1;
            	log_file = optarg;
            	log_fd = creat(log_file,0666);
            	if (log_fd == -1){
            		//error
			log_fd = open(log_file,O_RDWR);
			if (log_fd == -1){
            			fprintf(stderr, "error opening log file specified \n");
				exit(1);
			}
            	}
            	break;


            case 'i':
                id_flag = 1;
                if(strlen(optarg) != 9){
                    fprintf(stderr, "invalid id number given \n");
                }
                id = atoi(optarg);
                break;

            case 'h':
                host_flag = 1;
                hostname = optarg;
                break;

            default:
            	fprintf(stderr, "error with input \n");
            	exit(1);
            	break;
        }

    }

   if (!  argv[optind] ){
	fprintf(stderr, "no port number given\n");
	exit(1);
	}

    port_num = atoi(argv[optind]);
    if(port_num <= 0){

        fprintf(stderr, "invalid port number \n");
        exit(1);
    }

    if(log_flag == 0 || id == 0 || hostname == NULL){
        fprintf(stderr, "--log, --id, or --host are mandatory \n");
        exit(1);
    }


    close(STDIN_FILENO);
    tcp_init();
    SSL_init();
    char buf[256];
    sprintf(buf, "ID=%i\n", id);

    if(SSL_write(ssl, buf, strlen(buf)) == -1){
        fprintf(stderr, "error with SSL_write \n");
        exit(2);
    }

    dprintf(log_fd, "ID=%i\n", id);


    sensor = mraa_aio_init(1);
    if(sensor == NULL){
        fprintf(stderr, "error initializing temperature sensor \n");
	mraa_deinit();
        exit(1);
    }

    button = mraa_gpio_init(60);
    if(button == NULL){
        fprintf(stderr,"error initializing button \n");
	mraa_deinit();
    	exit(1);

    }

    mraa_gpio_dir(button, MRAA_GPIO_IN);


    
    pfd.fd =socket_fd;
    pfd.events = POLLIN | POLLHUP;

    int temp_reading = mraa_aio_read(sensor);
    float temperature = convert_temperature(temp_reading);
    time(&current);
    time_t next = current + period;
    print_temp(temperature);

    while (1){

    	if (current >= next){

	    	temp_reading = mraa_aio_read(sensor);
	        temperature = convert_temperature(temp_reading);
	

	        if (stop_flag != 1){

	        	print_temp(temperature);


	        }
	        next = current + period;

   		 }

   		if(mraa_gpio_read(button)){
            time(&current);
            print_current_time = localtime(&current);

            //dprintf(socket_fd, "%.2d:%.2d:%.2d SHUTDOWN\n", print_current_time->tm_hour, print_current_time->tm_min, print_current_time->tm_sec);






            if(log_flag){
                dprintf(log_fd, "%.2d:%.2d:%.2d SHUTDOWN\n", print_current_time->tm_hour, print_current_time->tm_min, print_current_time->tm_sec);    
            }

		mraa_aio_close(sensor);
    		mraa_gpio_close(button);
            exit(0);
        }


        int ret = poll(&pfd, 1, 0);
        if (ret == -1){
        	fprintf(stderr, "error with polling \n");
        }

        char buf[512];
        

        if (pfd.revents && POLLIN){

            int n = SSL_read(ssl, buf, 512);

            if ( n == -1){

                fprintf(stderr,"error reading input while polling \n");


            }

            int word_offset = 0;

            for (int i = 0; i < n; i++){
                if (buf[i] == '\n'){

                    buf[i] = 0;

                    if(strcmp(buf + word_offset, "OFF") == 0){
                        if(log_flag){
                            dprintf(log_fd, "%s\n",  buf+word_offset);
                        }

			time(&current);
                        print_current_time = localtime(&current);

                        char buf[256];
                        sprintf(buf, "%.2d:%.2d:%.2d SHUTDOWN\n", print_current_time->tm_hour, print_current_time->tm_min, print_current_time->tm_sec);
                       

                        if(SSL_write(ssl, buf, strlen(buf)) == -1){
                            fprintf(stderr, "error with SSL_write \n");
                            exit(2);
                        }



			              if(log_flag){
                            dprintf(log_fd, "%.2d:%.2d:%.2d SHUTDOWN\n", print_current_time->tm_hour, print_current_time->tm_min, print_current_time->tm_sec);    
                        }

			mraa_aio_close(sensor);
    			mraa_gpio_close(button);
                        exit(0);

                    }
                    else if(strncmp(buf + word_offset, "LOG", sizeof(char) * 3) == 0){ 
                        if(log_flag){
                            dprintf(log_fd, "%s\n",  buf+word_offset);
                        }

                    }
                    else if(strcmp(buf + word_offset, "STOP") == 0){
                        stop_flag = 1;
                        if(log_flag){
                            char *valid = buf + word_offset;
                            dprintf(log_fd, "%s\n",  valid);
                        }
                    }
                    else if(strcmp(buf + word_offset, "START") == 0){
                        stop_flag = 0;
                        if(log_flag){
                            char *valid = buf + word_offset;
                            dprintf(log_fd, "%s\n",  valid);
                        }
                    }
                    else if (strcmp(buf + word_offset, "SCALE=C") == 0){
                        temp_metric = 0;
                        if(log_flag){
                            char *valid = buf + word_offset;
                            dprintf(log_fd, "%s\n",  valid);
                        }
                    }
                    else if(strcmp(buf + word_offset, "SCALE=F")==0){
                        temp_metric = 1;
                        if(log_flag){
                            char *valid = buf + word_offset;
                            dprintf(log_fd, "%s\n",  valid);
                        }
                    }
                    else if(strncmp(buf + word_offset, "PERIOD=", sizeof(char) * 7) == 0){
                        period = (int) atoi(buf+word_offset+7);
                        if(log_flag){
                            char *valid = buf + word_offset;
                            dprintf(log_fd, "%s\n",  valid);
                        }

                    }
                    else {

                        if(log_flag){
                            char *invalid = buf + word_offset;
                            dprintf(log_fd, "%s\n",  invalid);
                        }
                    }


                    word_offset = i + 1;



                }

            }


        }

        time(&current);
   		

    }
	
	mraa_aio_close(sensor);
    	mraa_gpio_close(button);


 }
