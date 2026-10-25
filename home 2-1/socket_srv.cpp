// ***************************************************************************
//
// Program example for labs in subject Operating Systems
//
// Petr Olivka, Dept. of Computer Science, petr.olivka@vsb.cz, 2026
//
// Example of socket server.
//
// This program is example of socket server and it allows to connect and serve
// the only one client.
// The mandatory argument of program is port number for listening.
//
// ***************************************************************************

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define STR_CLOSE   "close"
#define STR_QUIT    "quit"

// ***************************************************************************
// log messages

#define LOG_ERROR               0       // errors
#define LOG_INFO                1       // information and notifications
#define LOG_DEBUG               2       // debug messages

// debug flag
int g_debug = LOG_INFO;

void log_msg( int t_log_level, const char *t_form, ... )
{
    const char *out_fmt[] = {
            "ERR: (%d-%s) %s\n",
            "INF: %s\n",
            "DEB: %s\n" };

    if ( t_log_level && t_log_level > g_debug )
    {
        return;
    }

    char l_buf[ 4096 ];
    va_list l_arg;
    va_start( l_arg, t_form );

    vsnprintf( l_buf, sizeof( l_buf ), t_form, l_arg );

    va_end( l_arg );

    switch ( t_log_level )
    {
        case LOG_INFO:
        case LOG_DEBUG:
            fprintf( stdout, out_fmt[ t_log_level ], l_buf );
            break;

        case LOG_ERROR:
            fprintf( stderr, out_fmt[ t_log_level ], errno, strerror( errno ), l_buf );
            break;
    }
}

// ***************************************************************************
// help

const char g_help[] =
    "\n"
    "  Socket server example.\n"
    "\n"
    "  Use: %s [-h] [-d] port_number\n"
    "\n"
    "    -d  debug mode \n"
    "    -h  this help\n"
    "\n"
    ;

// ***************************************************************************

void serve_client(int l_sock_listen, int l_sock_client){
    pollfd l_read_poll[ 2 ];
    l_read_poll[ 0 ].fd = STDIN_FILENO;
    l_read_poll[ 0 ].events = POLLIN;
    l_read_poll[ 1 ].fd = l_sock_client;
    l_read_poll[ 1 ].events = POLLIN;

    int counter = 0;
    int total_solved = 0;
    char batch_cmd[1024] = "";

    while ( 1  )
        { // communication
            char l_buf[ 256 ];

            // select from fds
            int l_poll = poll( l_read_poll, 2, -1 );

            if ( l_poll < 0 )
            {
                log_msg( LOG_ERROR, "Function poll failed!" );
                exit( EXIT_FAILURE );
            }

            // data on stdin?
            if ( l_read_poll[ 0 ].revents & POLLIN )
            {
                // read data from stdin
                int l_len = read( STDIN_FILENO, l_buf, sizeof( l_buf ) );
                if ( l_len == 0 )
                {
                    log_msg( LOG_DEBUG, "Stdin closed." );
                    exit( EXIT_SUCCESS );
                }
                if ( l_len < 0 )
                {
                    log_msg( LOG_ERROR, "Unable to read data from stdin." );
                    exit( EXIT_FAILURE );
                }
                else
                    log_msg( LOG_DEBUG, "Read %d bytes from stdin.", l_len );

                // send data to client
                l_len = write( l_sock_client, l_buf, l_len );
                if ( l_len < 0 )
                {
                    log_msg( LOG_ERROR, "Unable to send data to client." );
                    exit( EXIT_FAILURE );
                }
                else
                    log_msg( LOG_DEBUG, "Sent %d bytes to client.", l_len );
            }
            // data from client?

            if ( l_read_poll[ 1 ].revents & POLLIN )
            {
                // read data from socket
                int l_len = read( l_sock_client, l_buf, sizeof( l_buf ) );
                if ( l_len == 0 )
                {
                    log_msg( LOG_DEBUG, "Client closed socket!" );
                    log_msg( LOG_INFO, "Solved %d problems", total_solved);
                    close( l_sock_client );
                    break;
                }
                else if ( l_len < 0 )
                {
                    log_msg( LOG_ERROR, "Unable to read data from client." );
                    close( l_sock_client );
                    break;
                }
                else
                    log_msg( LOG_DEBUG, "Read %d bytes from client.", l_len );

                // write data to client
                l_buf[l_len - 1] = '\0';

                char* pointer = l_buf;
                if(strncasecmp(l_buf, "REQ: ", 5) == 0){
                    pointer = l_buf + 5;
                }

                if ( !strncasecmp( pointer, "close", strlen( STR_CLOSE ) ) )
                {
                    log_msg( LOG_INFO, "Client sent 'close' request to close connection." );
                    close( l_sock_client );
                    log_msg( LOG_INFO, "Solved %d problems", total_solved);
                    log_msg( LOG_INFO, "Connection closed. Waiting for new client." );
                    break;
                }
                if ( !strncasecmp( pointer, "quit", strlen( STR_QUIT ) ) )
                {
                    close( l_sock_listen );
                    close( l_sock_client );
                    log_msg( LOG_INFO, "Solved %d problems", total_solved);
                    log_msg( LOG_INFO, "Request to 'quit' entered" );
                    exit( EXIT_SUCCESS );
                }

                char l_python_cmd[512];
                snprintf(l_python_cmd, sizeof(l_python_cmd), "print(\"%s=\", %s)\n", pointer, pointer);
                strcat(batch_cmd, l_python_cmd);
                counter++;
                total_solved++;

                if(counter == 3){
                    int pipe_in[2], pipe_out[2];
                    pipe(pipe_in);
                    pipe(pipe_out);

                    pid_t child_pid = fork();

                    if (child_pid == 0){
                        close(pipe_in[1]);
                        close(pipe_out[0]);

                        dup2(pipe_in[0], STDIN_FILENO);
                        dup2(pipe_out[1], STDOUT_FILENO);

                        execlp("python3", "python3", nullptr);
                    }
                    else {
                        close(pipe_in[0]);
                        close(pipe_out[1]);

                        write(pipe_in[1], batch_cmd, strlen(batch_cmd));
                        close(pipe_in[1]);

                        l_len = read(pipe_out[0], l_buf, sizeof(l_buf));
                        close(pipe_out[0]); 
                        
                        if (l_len > 0) {
                            write(l_sock_client, l_buf, l_len);
                            log_msg(LOG_DEBUG, "Odeslan vysledek klientovi.");
                        }
                    }
                    counter = 0;
                    batch_cmd[0] = '\0';
                }
            }
        } // while communication
    }

int main( int t_argc, char **t_argv )
{
    int l_port = 0;

    // parsing arguments
    for ( int inx = 1; inx < t_argc; inx++ )
    {
        if ( strcmp( t_argv[ inx ], "-d" ) == 0 )
        {
            g_debug = LOG_DEBUG;
        }

        if ( strcmp( t_argv[ inx ], "-h" ) == 0 )
        {
            printf( g_help, t_argv[ 0 ] );
            exit( EXIT_SUCCESS );
        }

        if ( *t_argv[ inx ] != '-' )
        {
            if ( l_port == 0 )
            {
                l_port = atoi( t_argv[ inx ] );
            }
        }
    }

    if ( l_port <= 0 )
    {
        log_msg( LOG_INFO, "Bad or missing port number %d!", l_port );
        printf( g_help, t_argv[ 0 ] );
        exit( EXIT_FAILURE );
    }

    log_msg( LOG_INFO, "Server will listen on port: %d.", l_port );

    // socket creation
    int l_sock_listen = socket( AF_INET, SOCK_STREAM, 0 );
    if ( l_sock_listen < 0 )
    {
        log_msg( LOG_ERROR, "Unable to create socket.");
        exit( EXIT_FAILURE );
    }

    in_addr l_addr_any = { INADDR_ANY };
    sockaddr_in l_srv_addr;
    l_srv_addr.sin_family = AF_INET;
    l_srv_addr.sin_port = htons( l_port );
    l_srv_addr.sin_addr = l_addr_any;

    // Enable the port number reusing
    int l_opt = 1;
    if ( setsockopt( l_sock_listen, SOL_SOCKET, SO_REUSEADDR, &l_opt, sizeof( l_opt ) ) < 0 )
    {
      log_msg( LOG_ERROR, "Unable to set socket option!" );
    }

    // assign port number to socket
    if ( bind( l_sock_listen, (const sockaddr * ) &l_srv_addr, sizeof( l_srv_addr ) ) < 0 )
    {
        log_msg( LOG_ERROR, "Bind failed!" );
        close( l_sock_listen );
        exit( EXIT_FAILURE );
    }

    // listenig on set port
    if ( listen( l_sock_listen, 1 ) < 0 )
    {
        log_msg( LOG_ERROR, "Unable to listen on given port!" );
        close( l_sock_listen );
        exit( EXIT_FAILURE );
    }

    log_msg( LOG_INFO, "Enter 'quit' to quit server." );

    // go! Two loops inside.
    while ( 1 )
    {
        int l_sock_client = -1;

        // list of fd sources
        pollfd l_read_poll[ 2 ];

        l_read_poll[ 0 ].fd = STDIN_FILENO;
        l_read_poll[ 0 ].events = POLLIN;
        l_read_poll[ 1 ].fd = l_sock_listen;
        l_read_poll[ 1 ].events = POLLIN;

        while ( 1 ) // wait for new client
        {
            // select from fds
            int l_poll = poll( l_read_poll, 2, -1 );

            if ( l_poll < 0 )
            {
                log_msg( LOG_ERROR, "Function poll failed!" );
                exit( EXIT_FAILURE );
            }

            if ( l_read_poll[ 0 ].revents & POLLIN )
            { // data on stdin
                char buf[ 128 ];
                
                int l_len = read( STDIN_FILENO, buf, sizeof( buf) );
                if ( l_len == 0 )
                {
                    log_msg( LOG_DEBUG, "Stdin closed." );
                    exit( EXIT_SUCCESS );
                }
                if ( l_len < 0 )
                {
                    log_msg( LOG_DEBUG, "Unable to read from stdin!" );
                    exit( EXIT_FAILURE );
                }

                log_msg( LOG_DEBUG, "Read %d bytes from stdin", l_len );
                // request to quit?
                if ( !strncmp( buf, STR_QUIT, strlen( STR_QUIT ) ) )
                {
                    log_msg( LOG_INFO, "Request to 'quit' entered.");
                    close( l_sock_listen );
                    exit( EXIT_SUCCESS );
                }
            }

            if ( l_read_poll[ 1 ].revents & POLLIN )
            { // new client?
                sockaddr_in l_rsa;
                int l_rsa_size = sizeof( l_rsa );
                // new connection
                l_sock_client = accept( l_sock_listen, ( sockaddr * ) &l_rsa, ( socklen_t * ) &l_rsa_size );
                if ( l_sock_client < 0 )
                {
                    log_msg( LOG_ERROR, "Unable to accept new client." );
                    close( l_sock_listen );
                    exit( EXIT_FAILURE );
                }
                uint l_lsa = sizeof( l_srv_addr );
                // my IP
                getsockname( l_sock_client, ( sockaddr * ) &l_srv_addr, &l_lsa );
                log_msg( LOG_INFO, "My IP: '%s'  port: %d",
                                 inet_ntoa( l_srv_addr.sin_addr ), ntohs( l_srv_addr.sin_port ) );
                // client IP
                getpeername( l_sock_client, ( sockaddr * ) &l_srv_addr, &l_lsa );
                log_msg( LOG_INFO, "Client IP: '%s'  port: %d",
                                 inet_ntoa( l_srv_addr.sin_addr ), ntohs( l_srv_addr.sin_port ) );

                break;
            }
        } // while wait for client

        pid_t pid = fork();

        if (pid < 0) {
            log_msg( LOG_ERROR, "Fork failed!");
        }
        else if (pid == 0){
            close(l_sock_listen);
            serve_client(l_sock_listen, l_sock_client);
            close(l_sock_client);
            exit(EXIT_SUCCESS);
        }
        else{
            close(l_sock_client);
        }

        // change source from sock_listen to sock_client
    }

    return 0;
} // main

