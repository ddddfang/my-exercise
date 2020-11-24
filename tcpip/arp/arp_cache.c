
#include "arp.h"
#include "main.h"

#define arp_cache_head (&arp_cache[0])
#define arp_cache_end (&arp_cache[ARP_CACHE_SZ])
static struct arpentry arp_cache[ARP_CACHE_SZ];
static pthread_mutex_t arp_cache_mutex = PTHREAD_MUTEX_INITIALIZER;


//get arp packet from queue, and send out through netdev interface
void arp_queue_send(struct arpentry *ae)
{
    struct pkbuf *pkb;
    while (!list_empty(&ae->ae_list)) {
        pkb = list_first_entry(&ae->ae_list, struct pkbuf, pk_list);
        list_del(ae->ae_list.next);
        DEBUG_PRINT("send pending packet\r\n");
        net_tx(ae->ae_dev, pkb, pkb->pk_len - (int)ETH_HDR_LEN, pkb->pk_pro, ae->ae_hwaddr);
    }
}

void arp_queue_drop(struct arpentry *ae)
{
    struct pkbuf *pkb;
    while (!list_empty(&ae->ae_list)) {
        pkb = list_first_entry(&ae->ae_list, struct pkbuf, pk_list);
        list_del(ae->ae_list.next);
        DEBUG_PRINT("drop pending packet");
        free_pkb(pkb);
    }
}

//其实就已经添加到 cache 了
struct arpentry *arp_entry_alloc(void)
{
    static int next = 0;    //static...
    int i;
    struct arpentry *ae;

    pthread_mutex_lock(&arp_cache_mutex);
    for (i = 0; i < ARP_CACHE_SZ; i++) {
        if (arp_cache[next].ae_state == ARP_STATE_FREE)
            break;
        next = (next + 1) % ARP_CACHE_SZ;
    }

    if (i >= ARP_CACHE_SZ) {// not found
        DEBUG_PRINT("arp cache is full");
        pthread_mutex_unlock(&arp_cache_mutex);
        return NULL;
    }
    ae = &arp_cache[next];
    ae->ae_dev = NULL;
    ae->ae_retry = ARP_REQ_RETRY;
    ae->ae_ttl = ARP_TTL_WAITTIME;
    ae->ae_state = ARP_STATE_WAITING;
    ae->ae_pro = ETH_P_IP;		// default protocol
    list_init(&ae->ae_list);

    next = (next + 1) % ARP_CACHE_SZ;   // for next time allocation
    pthread_mutex_unlock(&arp_cache_mutex);
    return ae;
}

//insert some values into some fields,
//在arp_entry_alloc的时候就是将arpentry添加到cache了
int arp_cache_insert(struct netdev *nd, unsigned short pro, unsigned int ipaddr, unsigned char *hwaddr)
{
    struct arpentry *ae = arp_entry_alloc();
    if (!ae) {
        return -1;
    }
    ae->ae_dev = nd;
    ae->ae_pro = pro;
    ae->ae_ttl = ARP_TTL_TIMEOUT;
    ae->ae_ipaddr = ipaddr;
    ae->ae_state = ARP_STATE_RESOLVED;
    memcpy(ae->ae_hwaddr, hwaddr, NETDEV_ALEN);
    return 0;
}

void arp_cache_init(void)
{
    int i;
    for (i = 0; i < ARP_CACHE_SZ; i++)
        arp_cache[i].ae_state = ARP_STATE_FREE;
    DEBUG_PRINT("ARP CACHE INIT");
}

struct arpentry *arp_cache_lookup(unsigned short pro, unsigned int ipaddr)
{
    struct arpentry *ae, *ret = NULL;
    pthread_mutex_lock(&arp_cache_mutex);
    DEBUG_PRINT("pro:%d "IPFMT"\r\n", pro, IP_FMT(ipaddr));
    for (ae = arp_cache_head; ae < arp_cache_end; ae++) {
        if (ae->ae_state == ARP_STATE_FREE) {
            continue;
        }
        if (ae->ae_pro == pro && ae->ae_ipaddr == ipaddr) {
            ret = ae;
            break;
        }
    }
    pthread_mutex_unlock(&arp_cache_mutex);
    return ret;
}

//struct arpentry *arp_lookup_resolv(unsigned short pro, unsigned int ipaddr)
//{
//    struct arpentry *ae;
//    ae = arp_cache_lookup(pro, ipaddr);
//    if (ae && ae->ae_pro == ARP_STATE_RESOLVED)
//        return ae;
//    return NULL;
//}

void arp_timer(int delta)
{
    struct arpentry *ae;

    pthread_mutex_lock(&arp_cache_mutex);
    for (ae = arp_cache_head; ae < arp_cache_end; ae++) {
        if (ae->ae_state == ARP_STATE_FREE)
            continue;
        ae->ae_ttl -= delta;
        if (ae->ae_ttl <= 0) {
            //1.after retry, and still cannot get reply, remove it.
            //2.has already resolved, keep it for a certain time, then remove it
            if ((ae->ae_state == ARP_STATE_WAITING && --ae->ae_retry < 0) || ae->ae_state == ARP_STATE_RESOLVED) {
                if (ae->ae_state == ARP_STATE_WAITING) {
                    arp_queue_drop(ae);
                }
                ae->ae_state = ARP_STATE_FREE;
            } else {
                /* retry arp request */
                ae->ae_ttl = ARP_TTL_WAITTIME;  //after arp_request, wait ARP_WAITTIMEs, then check again in timer
                pthread_mutex_unlock(&arp_cache_mutex);
                arp_request(ae);
                pthread_mutex_lock(&arp_cache_mutex);
            }
        }
    }
    pthread_mutex_unlock(&arp_cache_mutex);
}


static inline const char *arpstate(struct arpentry *ae)
{
    switch (ae->ae_state) {
        case ARP_STATE_FREE:
            return "Free";
        case ARP_STATE_WAITING:
            return "Waiting";
        case ARP_STATE_RESOLVED:
            return "Resolved";
        default:
            return "";
    }
}

static char *ipnfmt(unsigned int ipaddr)
{
    static char ipbuf[16];
    snprintf(ipbuf, 16, IPFMT, IP_FMT(ipaddr));
    return ipbuf;
}

//print state
void arp_cache_traverse(void)
{
    struct arpentry *ae;
    int first;

    pthread_mutex_lock(&arp_cache_mutex);
    first = 1;
    for (ae = arp_cache_head; ae < arp_cache_end; ae++) {
        if (ae->ae_state == ARP_STATE_FREE)
            continue;
        if (first) {
            printf("State    Timeout(s)  HWaddress         Address\n");
            first = 0;
        }
        printf("%-9s%-12d" MACFMT " %s\n",
            arpstate(ae), ((ae->ae_ttl < 0) ? 0 : ae->ae_ttl),
            MAC_FMT(ae->ae_hwaddr), ipnfmt(ae->ae_ipaddr));
    }
    pthread_mutex_unlock(&arp_cache_mutex);
}
