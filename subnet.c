#include "subnet.h"
#include <arpa/inet.h>  // for inet_addr
#include <netinet/in.h> // for ntohl
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// 32-bit number where all bits are set to 1
const uint32_t ALL_ONES_MASK = 0xFFFFFFFFU;

// The subnetting table
const uint16_t SUBNET_TABLE[3][9] = {
    {1, 2, 4, 8, 16, 32, 64, 128, 256},    // num_subnets
    {256, 128, 64, 32, 16, 8, 4, 2, 1},    // num_host_ids
    {24, 25, 26, 27, 28, 29, 30, 31, 32}}; // CIDR

// Classless Inter-Domain Routing.
// Array of predefined subnet mask dotted decimal representations
// https://docs.netgate.com/pfsense/en/latest/network/cidr.html
const char *subnet_mask_dd[] = {
    // "0.0.0.0",         // /0
    // "128.0.0.0",       // /1
    // "192.0.0.0",       // /2
    // "224.0.0.0",       // /3
    // "240.0.0.0",       // /4
    // "248.0.0.0",       // /5
    // "252.0.0.0",       // /6
    // "254.0.0.0",       // /7
    // "255.0.0.0",       // /8
    // "255.128.0.0",     // /9
    // "255.192.0.0",     // /10
    // "255.224.0.0",     // /11
    // "255.240.0.0",     // /12
    // "255.248.0.0",     // /13
    // "255.252.0.0",     // /14
    // "255.254.0.0",     // /15
    // "255.255.0.0",     // /16
    // "255.255.128.0",   // /17
    // "255.255.192.0",   // /18
    // "255.255.224.0",   // /19
    // "255.255.240.0",   // /20
    // "255.255.248.0",   // /21
    // "255.255.252.0",   // /22
    // "255.255.254.0",   // /23
    "255.255.255.0",   // /24
    "255.255.255.128", // /25
    "255.255.255.192", // /26
    "255.255.255.224", // /27
    "255.255.255.240", // /28
    "255.255.255.248", // /29
    "255.255.255.252", // /30
    "255.255.255.254", // /31
    "255.255.255.255"  // /32
};

Subnet create_subnet(uint32_t ip, uint8_t prefix_length) {
  return (Subnet){.ip = ip, .mask = ALL_ONES_MASK << (32 - prefix_length)};
}

Subnet create_subnet_from_string(const char *subnet_string) {
  Subnet subnet;

  // Split the subnet string into IP address and prefix length
  char ip_part[16];
  int prefix_length;
  if (sscanf(subnet_string, "%15[^/]/%d", ip_part, &prefix_length) != 2) {
    perror("Invalid subnet string format");
    exit(EXIT_FAILURE);
  }

  // Convert IP address string to integer
  uint32_t ip = inet_addr(ip_part);
  if (ip == INADDR_NONE) {
    perror("Invalid IP address format");
    exit(EXIT_FAILURE);
  }

  // Calculate subnet mask
  if (prefix_length <= 0 || prefix_length > 32) {
    perror("Invalid prefix length");
    exit(EXIT_FAILURE);
  }

  // Convert from network byte order to host byte order
  subnet.ip = ntohl(ip);
  subnet.mask = ALL_ONES_MASK << (32 - (uint32_t)prefix_length);
  return subnet;
}

char classify_ip(IPAddress addr) {
  // Check if the most significant bit is 0 (0xxx xxxx)
  if ((addr.ip & 0x80000000U) == 0) {
    return 'A'; // Class A
  }

  // Check if the two most significant bits are 10 (10xx xxxx)
  else if ((addr.ip & 0xC0000000U) == 0x80000000U) {
    return 'B'; // Class B
  }
  // Check if the three most significant bits are 110 (110x xxxx)
  else if ((addr.ip & 0xE0000000U) == 0xC0000000U) {
    return 'C'; // Class C
  }
  // Check if the four most significant bits are 1110 (1110 xxxx)
  else if ((addr.ip & 0xF0000000U) == 0xE0000000U) {
    return 'D'; // Class D (Multicast)
  }
  // If none of the above conditions are met
  else {
    return 'E'; // Class E (Reserved)
  }
}

void assignable_range(Subnet subnet, IPAddress *start, IPAddress *end) {
  start->ip = (subnet.ip & subnet.mask) + 1;
  start->ip_class = classify_ip(*start);

  end->ip = (subnet.ip | ~subnet.mask) - 1;
  end->ip_class = classify_ip(*end);
}

uint32_t compute_assignable_addresses(Subnet subnet) {
  // Subtract 2 to exclude network and broadcast addresses
  return (~subnet.mask) - 2;
}

IPV4 to_ipv4(uint32_t ip) {
  IPV4 addr;
  snprintf(addr.addr, 16, "%u.%u.%u.%u", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF,
           (ip >> 8) & 0xFF, ip & 0xFF);
  return addr;
}

void print_ips_in_subnet(Subnet subnet) {
  for (uint32_t ip = subnet.ip; ip <= (subnet.ip | ~subnet.mask); ip++) {
    printf("%u.%u.%u.%u\n", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF,
           (ip >> 8) & 0xFF, ip & 0xFF);
  }
}

IPV4 *get_ips_in_subnet(Subnet subnet, size_t *num_ips) {
  uint32_t start_ip = subnet.ip + 1;                // Exclude network address
  uint32_t end_ip = (subnet.ip | ~subnet.mask) - 1; // Exclude broadcast address
  size_t num_addresses = end_ip - start_ip + 1;

  IPV4 *ip_addresses = (IPV4 *)malloc(num_addresses * sizeof(IPV4));
  if (!ip_addresses) {
    perror("Memory allocation failed");
    return NULL;
  }

  for (size_t i = 0; i < num_addresses; i++) {
    IPV4 ipv4;
    snprintf(ipv4.addr, 16, "%u.%u.%u.%u", (start_ip >> 24) & 0xFF,
             (start_ip >> 16) & 0xFF, (start_ip >> 8) & 0xFF, start_ip & 0xFF);
    ip_addresses[i] = ipv4;
    start_ip++;
  }

  *num_ips = num_addresses;
  return ip_addresses;
}

SubnetInfo *get_subnet_table(char *networkID, uint16_t num_subnets) {
  // Sanity checks
  if (num_subnets < 1 || num_subnets > 256) {
    printf("num_subnets must be between 1 and 256\n");
    return NULL;
  }

  // Find the closest power of 2 that is greater than or equal to num_subnets
  uint16_t nearest_subnet = 1;
  uint16_t nearest_index = 0;
  while (nearest_subnet < num_subnets) {
    nearest_subnet <<= 1;
    nearest_index++;
  }

  Subnet subnet = create_subnet_from_string(networkID);

  uint16_t total_num_hosts = SUBNET_TABLE[1][nearest_index];
  uint16_t new_subnet_mask = SUBNET_TABLE[2][nearest_index];

  uint32_t networkID_int = subnet.ip;

  SubnetInfo *subnet_info = malloc(sizeof(SubnetInfo) * num_subnets);
  if (!subnet_info) {
    perror("unable to allocate enough memory for SubnetInfo array");
    return NULL;
  }

  for (uint16_t i = 0; i < num_subnets; i++) {
    uint32_t start_range = (networkID_int + 1);
    uint32_t end_range = (networkID_int + total_num_hosts - 2);

    SubnetInfo info = {.data.num_usable_hosts = total_num_hosts - 2};

    snprintf(info.data.network_id, sizeof(info.data.network_id), "%u.%u.%u.%u",
             (networkID_int >> 24) & 0xFF, (networkID_int >> 16) & 0xFF,
             (networkID_int >> 8) & 0xFF, networkID_int & 0xFF);

    // Find the index of new_subnet_mask_value in your predefined list
    int subnet_mask_index = new_subnet_mask - 24;

    snprintf(info.subnet_mask, sizeof(info.subnet_mask), "/%d (%s)",
             new_subnet_mask, subnet_mask_dd[subnet_mask_index]);

    snprintf(info.data.host_range_start, sizeof(info.data.host_range_start),
             "%u.%u.%u.%u", (start_range >> 24) & 0xFF,
             (start_range >> 16) & 0xFF, (start_range >> 8) & 0xFF,
             start_range & 0xFF);

    snprintf(info.data.host_range_end, sizeof(info.data.host_range_end),
             "%u.%u.%u.%u", (end_range >> 24) & 0xFF, (end_range >> 16) & 0xFF,
             (end_range >> 8) & 0xFF, end_range & 0xFF);

    snprintf(info.data.broadcast_id, sizeof(info.data.broadcast_id),
             "%u.%u.%u.%u", (networkID_int + total_num_hosts - 1) >> 24 & 0xFF,
             (networkID_int + total_num_hosts - 1) >> 16 & 0xFF,
             (networkID_int + total_num_hosts - 1) >> 8 & 0xFF,
             (networkID_int + total_num_hosts - 1) & 0xFF);

    subnet_info[i] = info;
    networkID_int += total_num_hosts;
  }

  return subnet_info;
}

void print_subnet_table(SubnetInfo *subnet_info, uint16_t num_subnets) {
  // Print table header
  printf("%s\n", "-------------------------------------------------------------"
                 "-----------------------");
  printf("%-18s | %-33s | %-10s | %-18s\n", "Network ID", "Host ID Range",
         "# Usable", "Broadcast ID");

  // Print separator line
  printf("%s\n", "-------------------------------------------------------------"
                 "-----------------------");

  // Print subnet information
  for (uint16_t i = 0; i < num_subnets; i++) {
    printf("%-18s | %-15s - %-15s | %-10u | %-18s\n",
           subnet_info[i].data.network_id, subnet_info[i].data.host_range_start,
           subnet_info[i].data.host_range_end,
           subnet_info[i].data.num_usable_hosts,
           subnet_info[i].data.broadcast_id);
  }
}
