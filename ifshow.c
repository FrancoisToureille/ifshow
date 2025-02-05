#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>

// Fonction qui affiche les adresses IP et leurs préfixes d'une interface réseau spécifique
void show_interface_addresses(const char *ifname) {
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || strcmp(ifa->ifa_name, ifname) != 0)
            continue;

        int family = ifa->ifa_addr->sa_family;
        char addr[INET6_ADDRSTRLEN];
        int prefix_length = -1;

        if (family == AF_INET || family == AF_INET6) {
            void *addr_ptr = (family == AF_INET) ?
                             (void *)&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr :
                             (void *)&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            inet_ntop(family, addr_ptr, addr, sizeof(addr));
            
            if (ifa->ifa_netmask) {
                if (family == AF_INET) {
                    prefix_length = __builtin_popcount(((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr);
                } else {
                    struct in6_addr *mask = &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr;
                    for (int i = 0; i < 16; i++) {
                        prefix_length += __builtin_popcount(mask->s6_addr[i]);
                    }
                }
            }
            
            printf("%s: %s/%d\n", ifa->ifa_name, addr, prefix_length);
        }
    }
    
    freeifaddrs(ifaddr);
}

// Fonction qui affiche toutes les interfaces réseau et leurs adresses IP avec préfixes
void show_all_interfaces() {
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        int family = ifa->ifa_addr->sa_family;
        char addr[INET6_ADDRSTRLEN];
        int prefix_length = -1;
        
        if (family == AF_INET || family == AF_INET6) {
            void *addr_ptr = (family == AF_INET) ?
                             (void *)&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr :
                             (void *)&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            inet_ntop(family, addr_ptr, addr, sizeof(addr));
            
            if (ifa->ifa_netmask) {
                if (family == AF_INET) {
                    prefix_length = __builtin_popcount(((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr.s_addr);
                } else {
                    struct in6_addr *mask = &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr;
                    for (int i = 0; i < 16; i++) {
                        prefix_length += __builtin_popcount(mask->s6_addr[i]);
                    }
                }
            }
            
            printf("%s: %s/%d\n", ifa->ifa_name, addr, prefix_length);
        }
    }
    
    freeifaddrs(ifaddr);
}

// Fonction principale
int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        show_interface_addresses(argv[2]);
    } 
    else if (argc == 2 && strcmp(argv[1], "-a") == 0) {
        show_all_interfaces();
    } 
    else {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s -i <interface> : Show IP addresses and prefixes of specified interface\n", argv[0]);
        fprintf(stderr, "  %s -a : Show all interfaces with their IP addresses and prefixes\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}