#ifndef __SUBNET_H__
#define __SUBNET_H__

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint32_t ip;
  uint32_t mask;
} Subnet;

typedef struct {
  uint32_t ip;
  char ip_class;
} IPAddress;

typedef struct SubnetInfo {
  struct {
    char network_id[16];
    char host_range_start[16];
    char host_range_end[16];
    char broadcast_id[16];
    uint32_t num_usable_hosts;
  } data;
  char subnet_mask[24];
} SubnetInfo;

typedef struct {
  char addr[16];
} IPV4;

Subnet create_subnet(uint32_t ip, uint8_t prefix_length);
Subnet create_subnet_from_string(const char *subnet_string);
char classify_ip(IPAddress addr);
void assignable_range(Subnet subnet, IPAddress *start, IPAddress *end);
uint32_t compute_assignable_addresses(Subnet subnet);

// convert ip to string. This allocates 16 bytes that must be freed.
IPV4 to_ipv4(uint32_t ip);

// Print all IP addresses in given subnet
void print_ips_in_subnet(Subnet subnet);

// Allocates an array for num_ips on heap that must be freed.
// Call free() once since all strings are allocated in a single linear memory.
IPV4 *get_ips_in_subnet(Subnet subnet, size_t *num_ips);

// Free array of dynamically allocated ip addresses.
void free_ip_addresses(char **ip_addresses, size_t num_ips);

// Returns an a dynamically allocated array of Subnets for a given network id.
// e.g 192.168.0/24
SubnetInfo *get_subnet_table(char *network_id, uint16_t num_subnets);

// print subnet table from the subnet_info array.
void print_subnet_table(SubnetInfo *subnet_info, uint16_t num_subnets);

#endif /* __SUBNET_H__ */
