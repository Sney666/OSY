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
#include <thread>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 4
#define ICONS_COUNT 8

const char *icons[ICONS_COUNT] = {
    "🐱", "🐶", "🐭", "🐹", "🐰", "🦊", "🐻", "🐼"
    /*"🐨", "🐯", "🦁", "🐮", "🐷", "🐸", "🐵", "🐔",
    "🐧", "🐦", "🐤", "🐺", "🦄", "🐝", "🐞", "🦋",
    "🐢", "🐍", "🦖", "🐙", "🐠", "🐬", "🦆", "🐊"*/
};

int board[SIZE][SIZE];

struct Client
{
    int fd;
    std::string nick;
    int score;
};

std::vector<Client> g_clients;

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

void init_board(void) {
    int pairs[SIZE * SIZE];
    int n = 0;

    // vytvoř 2 kopie každé ikonky
    for (int i = 0; i < ICONS_COUNT; i++) {
        pairs[n++] = i + 1;
        pairs[n++] = i + 1;
    }

    // zamíchej pole
    srand(time(NULL));
    for (int i = SIZE * SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = pairs[i];
        pairs[i] = pairs[j];
        pairs[j] = tmp;
    }

    // naplň hrací desku zápornými hodnotami (skryté)
    n = 0;
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            board[r][c] = -pairs[n++];
}

std::string get_board_str(void) {
    std::string out = "      ";
    char buf[64];

    for (int c = 0; c < SIZE; c++) {
        snprintf(buf, sizeof(buf), "  %c   ", 'A' + c);
        out += buf;
    }
    out += "\n    +";

    for (int c = 0; c < SIZE; c++)
        out += "-----+";
    out += "\n";

    for (int r = 0; r < SIZE; r++) {
        snprintf(buf, sizeof(buf), " %2d |", r + 1);
        out += buf;
        
        for (int c = 0; c < SIZE; c++) {
            int v = board[r][c];
            if (v == 0) {
                out += "     |";
            } else if (v < 0) {
                out += " ??  |";
            } else {
                out += " ";
                out += icons[v - 1];
                out += "  |";
            }
        }
        out += "\n    +";
        for (int c = 0; c < SIZE; c++)
            out += "-----+";
        out += "\n";
    }
    return out;
}

void handle_client(int client_fd)
{
    char l_buf[256];
    char nick_buf[256];

    int n_len = read(client_fd, l_buf, sizeof(l_buf) - 1);
    if (n_len <= 0) {
        log_msg(LOG_INFO, "Klient se odpojil jeste pred prihlasenim.");
        close(client_fd);
        return;
    }

    l_buf[n_len] = '\0';

    if (sscanf(l_buf, "HELLO %255s", nick_buf) == 1) {}
    else {
        strcpy(nick_buf, "Neznamy");
        log_msg(LOG_DEBUG, "Klient poslal nespravny uvitaci format: %s", l_buf);
    }

    Client new_client;
    new_client.fd = client_fd;
    new_client.nick = nick_buf;
    new_client.score = 0;
    
    g_clients.push_back(new_client);
    log_msg(LOG_INFO, "Novy klient (fd: %d) pridan s nickem: %s", client_fd, new_client.nick.c_str());

    char response[256];
    snprintf(response, sizeof(response), "WELCOME TO THE GAME, %s\n", new_client.nick.c_str());
    write(client_fd, response, strlen(response));
    
    std::string board_text = get_board_str();
    write(client_fd, board_text.c_str(), board_text.length());
    
    while (1)
    {
        // Čteme data od klienta
        int l_len = read(client_fd, l_buf, sizeof(l_buf) - 1);
        
        if (l_len <= 0 || !strncasecmp(l_buf, STR_CLOSE, strlen(STR_CLOSE)))
        {
            log_msg(LOG_INFO, "Klient (fd: %d) se odpojil nebo nastala chyba.", client_fd);
            close(client_fd);
            for (auto it = g_clients.begin(); it != g_clients.end(); ++it)
            {
                if (it->fd == client_fd)
                {
                    g_clients.erase(it);
                    log_msg(LOG_DEBUG, "Klient smazan ze seznamu.");
                    break; 
                }
            }
            
            break;
        }
        
        l_buf[l_len] = '\0';
        
        char c1, c2;
        int r1, r2;

        if (sscanf(l_buf, "REVEAL %c%d %c%d", &c1, &r1, &c2, &r2) == 4) {
            
            int col1 = c1 - 'A';
            int row1 = r1 - 1;
            int col2 = c2 - 'A';
            int row2 = r2 - 1;

            if (row1 >= 0 && row1 < SIZE && col1 >= 0 && col1 < SIZE &&
                row2 >= 0 && row2 < SIZE && col2 >= 0 && col2 < SIZE &&
                !(row1 == row2 && col1 == col2)) {
                
                if (board[row1][col1] < 0 && board[row2][col2] < 0) {
                    
                    board[row1][col1] = -board[row1][col1];
                    board[row2][col2] = -board[row2][col2];

                    std::string temp_board = get_board_str();
                    for (auto it = g_clients.begin(); it != g_clients.end(); ++it) {
                        write(it->fd, temp_board.c_str(), temp_board.length());
                    }

                    if (board[row1][col1] == board[row2][col2]) {
                        for (auto &client : g_clients) {
                            if (client.fd == client_fd) {
                                client.score++;
                                break;
                            }
                        }

                        board[row1][col1] = 0;
                        board[row2][col2] = 0;

                        int total_pairs_found = 0;
                        for (const auto &client : g_clients) {
                            total_pairs_found += client.score;
                        }

                        std::string found = "PAIR FOUND\n";
                        write(client_fd, found.c_str(), found.length());

                        if(total_pairs_found == ICONS_COUNT) {
                            std::string final_board = get_board_str();
                            
                            std::string results = "\n=== KONEC HRY ===\nVsechny pary byly odhaleny!\nSkore hracu:\n";
                            for (const auto &client : g_clients) {
                                char score_msg[256];
                                snprintf(score_msg, sizeof(score_msg), "%s: %d bodu\n", client.nick.c_str(), client.score);
                                results += score_msg;
                            }

                            for (auto it = g_clients.begin(); it != g_clients.end(); ++it) {
                                write(it->fd, final_board.c_str(), final_board.length());
                                write(it->fd, results.c_str(), results.length());
                            }
                            
                            init_board();
                            for(auto &c : g_clients) c.score = 0;
                            continue;
                        }
                    } else {
                        sleep(2);
                        board[row1][col1] = -board[row1][col1];
                        board[row2][col2] = -board[row2][col2];
                        std::string no_match = "NO MATCH\n";
                        write(client_fd, no_match.c_str(), no_match.length());
                    }

                    std::string final_board = get_board_str();
                    for (auto it = g_clients.begin(); it != g_clients.end(); ++it) {
                        write(it->fd, final_board.c_str(), final_board.length());
                    }
                }
                else{
                    std::string invalid = "INVALID\n";
                    write(client_fd, invalid.c_str(), invalid.length());
                }
            }
            else{
                std::string invalid = "INVALID\n";
                write(client_fd, invalid.c_str(), invalid.length());
            }
        }
        else{
            std::string invalid = "INVALID\n";
            write(client_fd, invalid.c_str(), invalid.length());
        }
    }
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

        int l_sock_client = -1;

        // list of fd sources
        pollfd l_read_poll[ 2 ];

        l_read_poll[ 0 ].fd = STDIN_FILENO;
        l_read_poll[ 0 ].events = POLLIN;
        l_read_poll[ 1 ].fd = l_sock_listen;
        l_read_poll[ 1 ].events = POLLIN;

        init_board();
        std::string initial_board = get_board_str();
        printf("%s\n", initial_board.c_str());

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

                buf[l_len] = '\0';
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "Server: %s", buf);
                for(auto it = g_clients.begin(); it != g_clients.end(); ++it){
                    write( it->fd, buffer, strlen(buffer));
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

                std::thread(handle_client, l_sock_client).detach();
            }

        } // while wait for client

    return 0;
// main
}

