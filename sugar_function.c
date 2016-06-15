/*************************************************************************
	> File Name: sugar_function.c
	> Author: 
	> Mail: 
	> Created Time: Wed 25 May 2016 08:41:58 PM PDT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sugarsoapBinding.nsmap"
#include "sugar_global.h"

char *leave_number(char *dest, char *src)
{
    if(src == NULL){
        return NULL;
    }
    char *p = dest;
    while(*src) {
        if (*src > '9' || *src <  '0') {
            src++;
        } else {
            *dest = *src;
            dest++;
            src++;
        }
    }
    *dest = '\0';
    return p;
}

char *addPercent(char *dest, char *src)
{
    if (src == NULL) {
        return NULL;
    }
    char *p = dest;
    while(*src) {
        *dest++ = '%';
        *dest++ = *src++;
    }
    *dest = '\0';
    return p;
}


void queueInitialize(Queue *queue)
{
    QueueNode *q_head_node = malloc(sizeof(QueueNode));
    if (q_head_node)
    {
        q_head_node->value = NULL;
        q_head_node->next = NULL;
    }

    queue->head = queue->end = q_head_node;
    queue->not_empty = 0;

    pthread_mutex_init(&queue->q_h_lock, NULL);
    pthread_mutex_init(&queue->q_t_lock, NULL);
}

void cleanQueue(Queue *queue)
{
    QueueNode *free_q_head_node = NULL;

    if (queue)
    {
        while(queue->head != NULL)
        {
            free_q_head_node = queue->head->next;
            if (queue->head->value != NULL)
            {
                cleanSGCRMTask(queue->head->value);
                queue->head->value = NULL;
            }

            free(queue->head);
            queue->head = free_q_head_node;
        }
    }
}

ENQUEUE_RESULT enqueueTask(Queue *q, SGCRMTask *task)
{
    QueueNode* node = malloc(sizeof(QueueNode));
    if (node != NULL)
    {
        node->value = task;
        node->next = NULL;
    }

    pthread_mutex_lock(&q->q_t_lock);
    if (q->end != NULL)
    {
        q->end->next = node;
        q->end = node;
    }
    else
    {
        q->not_empty = 0;
        pthread_mutex_unlock(&q->q_t_lock);
        return ENQUEUE_FAILED_USER_DELETE;
    }
    q->not_empty = 1;
    pthread_mutex_unlock(&q->q_t_lock);

    return ENQUEUE_SUCCESS;
}

DEQUEUE_RESULT dequeueTask(Queue *q, SGCRMTask **pvalue)
{
    DEQUEUE_RESULT ret;
    QueueNode *next_node = NULL;

    pthread_mutex_lock(&q->q_h_lock);
    if (0 == q->not_empty)
    {
        pthread_mutex_unlock(&q->q_h_lock);
        return DEQUEUE_FAILED_EMPTY;
    }

    if(q->head != NULL)
    {
        next_node = q->head->next ;
    }

    if (next_node != NULL)
    {
        if (next_node == q->end)
        {
            pthread_mutex_lock(&q->q_t_lock);
            q->end = q->head;
            *pvalue = next_node->value;

            q->not_empty = 0;
			q->head->next = NULL;
            ret = DEQUEUE_SUCCESS_AND_EMPTY;
            pthread_mutex_unlock(&q->q_t_lock);
        }
        else
        {
            *pvalue = next_node->value;
            q->head->next = next_node->next ;
            ret = DEQUEUE_SUCCESS_AND_CONTINUE;
        }

        pthread_mutex_unlock(&q->q_h_lock);
        free(next_node);
    }
    else
    {
        pthread_mutex_unlock(&q->q_h_lock);
        ret = DEQUEUE_FAILED_EMPTY;
    }

    return ret;
}

struct __sg_crm_info_task *initSGCRMInfoTask(struct __sg_crm_info_task *crm_info_task)
{
    crm_info_task = (struct __sg_crm_info_task *)malloc(sizeof(struct __sg_crm_info_task));
    crm_info_task->login_return = initSGLoginReturn(crm_info_task->login_return);
    crm_info_task->server_info = initSGServerInfo(crm_info_task->server_info);
    return crm_info_task;
}

void cleanSGCRMInfoTask(struct __sg_crm_info_task *crm_info_task)
{
    if (crm_info_task)
    {
        cleanSGLoginReturn(crm_info_task->login_return);
        cleanSGServerInfo(crm_info_task->server_info);
    }
    SAFE_FREE(crm_info_task);
}

struct __sg_crm_task *initSGCRMTask(struct __sg_crm_task *crm_task)
{
    crm_task = (struct __sg_crm_task *)malloc(sizeof(struct __sg_crm_task));
    crm_task->search_result = NULL;
    crm_task->call_item = NULL;
    crm_task->link_return = NULL;
    return crm_task;
}

void cleanSGCRMTask(struct __sg_crm_task *crm_task)
{
    if (crm_task)
    {
        cleanSGSearchResult(crm_task->search_result);
        cleanSGCallItem(crm_task->call_item);
        cleanSGLinkReturn(crm_task->link_return);
    }
    SAFE_FREE(crm_task);
}

struct __sg_search_result *initSGSearchResult(struct __sg_search_result *search_result) 
{
    search_result = (struct __sg_search_result *)malloc(sizeof(struct __sg_search_result));
    search_result->id = NULL;
    search_result->module = NULL;
    return search_result;
}

void cleanSGSearchResult(struct __sg_search_result *search_result)
{
    printf("cleanSearchResult\n");
    if (search_result) {
		printf("youyou\n");
		if (search_result->id)
			printf("asdjfsadfasd\n");
		else
			printf("no nulllllll]n\n");
        SAFE_FREE(search_result->id);
        SAFE_FREE(search_result->module);
		printf("ono\n");
    }
    SAFE_FREE(search_result);
	printf("cleanSearchResult end\n");
}

struct __sg_call_item *initSGCallItem(struct __sg_call_item *call_item)
{
    call_item = (struct __sg_call_item*)malloc(sizeof(struct __sg_call_item));
    return call_item;
}

void cleanSGCallItem(struct __sg_call_item* call_item)
{
    if (call_item) {
		printf("cleanCallItem\n");
        SAFE_FREE(call_item->name);
        SAFE_FREE(call_item->parent_type);
        SAFE_FREE(call_item->parent_id);
        SAFE_FREE(call_item->direction);
        SAFE_FREE(call_item->description);
        SAFE_FREE(call_item->duration_hours);
        SAFE_FREE(call_item->duration_minutes);
        SAFE_FREE(call_item->date_start);
        SAFE_FREE(call_item->assigned_user_id);
    }
    SAFE_FREE(call_item);
}

struct __sg_contact_item *initSGContactItem(struct __sg_contact_item *contact_item)
{
	contact_item = (struct __sg_contact_item*)malloc(sizeof(struct __sg_contact_item));
	contact_item->title = NULL;
	contact_item->first_name = NULL;
	contact_item->last_name = NULL;
	contact_item->phone_home = NULL;
	contact_item->phone_work = NULL;
	contact_item->phone_mobile = NULL;
	contact_item->phone_other = NULL;
	contact_item->phone_fax = NULL;
	return contact_item;
}

void cleanSGContactItem(struct __sg_contact_item *contact_item)
{
    if (contact_item) {
        SAFE_FREE(contact_item->title);
        SAFE_FREE(contact_item->first_name);
        SAFE_FREE(contact_item->last_name);
        SAFE_FREE(contact_item->phone_home);
        SAFE_FREE(contact_item->phone_work);
        SAFE_FREE(contact_item->phone_mobile);
        SAFE_FREE(contact_item->phone_other);
        SAFE_FREE(contact_item->phone_fax);
		SAFE_FREE(contact_item->description);
    }
    SAFE_FREE(contact_item);
}
struct __sg_account_item *initSGAccountItem(struct __sg_account_item *account_item)
{
	account_item = (struct __sg_account_item*)malloc(sizeof(struct __sg_account_item));
	account_item->name = NULL;
	account_item->phone_office = NULL;
	account_item->phone_alternate = NULL;
	account_item->phone_fax = NULL;
	account_item->description = NULL;
	return account_item;
}

void cleanSGAccountItem(struct __sg_account_item *account_item)
{
	if (account_item) {
        SAFE_FREE(account_item->name);
        SAFE_FREE(account_item->phone_office);
        SAFE_FREE(account_item->phone_alternate);
        SAFE_FREE(account_item->phone_fax);
		SAFE_FREE(account_item->description);
    }
    SAFE_FREE(account_item);
}

struct __sg_lead_item *initSGLeadItem(struct __sg_lead_item *lead_item)
{
	lead_item = (struct __sg_lead_item*)malloc(sizeof(struct __sg_lead_item));
	lead_item->title = NULL;
	lead_item->first_name = NULL;
	lead_item->last_name = NULL;
	lead_item->phone_home = NULL;
	lead_item->phone_work = NULL;
	lead_item->phone_mobile = NULL;
	lead_item->phone_other = NULL;
	lead_item->phone_fax = NULL;
	lead_item->description = NULL;
	return lead_item;
}
void cleanSGLeadItem(struct __sg_lead_item *lead_item)
{
	if (lead_item) {
        SAFE_FREE(lead_item->title);
        SAFE_FREE(lead_item->first_name);
        SAFE_FREE(lead_item->last_name);
        SAFE_FREE(lead_item->phone_home);
        SAFE_FREE(lead_item->phone_work);
        SAFE_FREE(lead_item->phone_mobile);
        SAFE_FREE(lead_item->phone_other);
        SAFE_FREE(lead_item->phone_fax);
		SAFE_FREE(lead_item->description);
    }
    SAFE_FREE(lead_item);
}

struct __sg_server_info *initSGServerInfo(struct __sg_server_info *server_info)
{
    server_info = (struct __sg_server_info *)malloc(sizeof(struct __sg_server_info));
    return server_info;
}

void cleanSGServerInfo(struct __sg_server_info *server_info)
{
    if (server_info) {
        //SAFE_FREE(server_info->soap);//soap need use soap destory
        SAFE_FREE(server_info->soap_endpoint);
    }
    SAFE_FREE(server_info);
}
struct __sg_login_return *initSGLoginReturn(struct __sg_login_return *login_return)
{
    login_return = (struct __sg_login_return *)malloc(sizeof(struct __sg_login_return));
    login_return->session_id = NULL;
    login_return->user_id = NULL;
    return login_return;
}

void cleanSGLoginReturn(struct __sg_login_return *login_return)
{
    if (login_return) {
        SAFE_FREE(login_return->session_id);
        SAFE_FREE(login_return->user_id);
    }
    SAFE_FREE(login_return);
}

struct __sg_link_return *initSGLinkReturn(struct __sg_link_return *link_return)
{
    link_return = (struct __sg_link_return *)malloc(sizeof(struct __sg_link_return));
    return link_return;
}

void cleanSGLinkReturn(struct __sg_link_return *link_return)
{
    printf("cleanLinkReturn\n");
    SAFE_FREE(link_return);
}

struct __sg_calls_item *initSGCallsItem(struct __sg_calls_item *calls_item)
{
	calls_item = (struct __sg_calls_item *)malloc(sizeof(struct __sg_calls_item));
	calls_item->call_id = NULL;
	calls_item->call_item = initSGCallItem(calls_item->call_item);
	calls_item->next = NULL;
	return calls_item;
}
void cleanSGCallsItem(struct __sg_calls_item *calls_item)
{
	struct __sg_calls_item *tmp_node = calls_item;
	while (calls_item)
	{
		if (calls_item->next)
		{
			tmp_node = calls_item->next;
			SAFE_FREE(calls_item->call_id);
			cleanSGCallItem(calls_item->call_item);
			SAFE_FREE(calls_item);
			calls_item = tmp_node;
		}
		else
		{
			SAFE_FREE(calls_item->call_id);
			cleanSGCallItem(calls_item->call_item);
			free(calls_item);
			calls_item = NULL;
		}
	}
}


struct __sg_contacts_id *initSGContactsID(struct __sg_contacts_id *contacts_id)
{
	contacts_id = (struct __sg_contacts_id *)malloc(sizeof(struct __sg_contacts_id));
	contacts_id->id = NULL;
	contacts_id->next = NULL;
	return contacts_id;
}
void cleanSGContactsID(struct __sg_contacts_id *contacts_id)
{
	struct __sg_contacts_id *tmp_node = contacts_id;
	while (contacts_id)
	{
		if (contacts_id->next)
		{
			tmp_node = contacts_id->next;
			SAFE_FREE(contacts_id->id);
			SAFE_FREE(contacts_id);
			contacts_id = tmp_node;
		}
		else
		{
			SAFE_FREE(contacts_id->id);
			free(contacts_id);
			contacts_id = NULL;
		}
	}
}
struct __sg_links_id *initSGLinksID(struct __sg_links_id *links_id)
{
	links_id = (struct __sg_links_id *)malloc(sizeof(struct __sg_links_id));
	links_id->id = NULL;
	links_id->next = NULL;
	return links_id;
}
void cleanSGLinksID(struct __sg_links_id *links_id)
{
	struct __sg_links_id *tmp_node = links_id;
	while (links_id)
	{
		if (links_id->next)
		{
			tmp_node = links_id->next;
			SAFE_FREE(links_id->id);
			SAFE_FREE(links_id);
			links_id = tmp_node;
		}
		else
		{
			SAFE_FREE(links_id->id);
			free(links_id);
			links_id = NULL;
		}
	}
}

struct __sg_return_calls_id *initSGReturnCallsID(struct __sg_return_calls_id *return_calls_id)
{
	return_calls_id = (struct __sg_return_calls_id *)malloc(sizeof(struct __sg_return_calls_id));
	return_calls_id->return_call_id = NULL;
	return_calls_id->next = NULL;
	return return_calls_id;
}
void cleanSGReturnCallsID(struct __sg_return_calls_id *return_calls_id)
{
	struct __sg_return_calls_id *tmp_node = return_calls_id;
	while (return_calls_id)
	{
		if (return_calls_id->next)
		{
			tmp_node = return_calls_id->next;
			SAFE_FREE(return_calls_id->return_call_id);
			SAFE_FREE(return_calls_id);
			return_calls_id = tmp_node;
		}
		else
		{
			SAFE_FREE(return_calls_id->return_call_id);
			free(return_calls_id);
			return_calls_id = NULL;
		}
	}
}


/* ******** pack call functoin ****** */
int sugarLogin(struct soap *soap, const char *soap_endpoint, char *login_name, char *login_pwd, struct ns1__loginResponse *_param_1)
{
    char soap_action[128] = {0};

    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/login", soap_endpoint);
    struct ns1__user_USCOREauth *user_auth = NULL;
    struct name_USCOREvalue_USCORElist *name_value_list = NULL;
    user_auth = (struct ns1__user_USCOREauth*)malloc(sizeof(struct ns1__user_USCOREauth));
    
    char *username = (char*)malloc(256);
    char *password = (char*)malloc(512);
    
    snprintf(username, 256, "%s", login_name);
    char *md_res = MD_5(login_pwd);
    snprintf(password, 512, "%s", md_res);
    //printf("%s\n", password);
    if (md_res) {
        free(md_res);
        md_res = NULL;
    }

    user_auth->user_USCOREname = username;
    user_auth->password = password;
    char *application_name = "UCM6XXX";
    
    int ret = soap_call_ns1__login(soap, soap_endpoint, soap_action, user_auth, application_name, name_value_list, _param_1);

    //SAFE_FREE(username);
    //SAFE_FREE(password);
	if (user_auth)
	{
		SAFE_FREE(user_auth->user_USCOREname);
		SAFE_FREE(user_auth->password);
		free(user_auth);
	}
    return ret;
}

int sugarLogout(struct soap *soap, const char *soap_endpoint, char *session, struct ns1__logoutResponse *_param_2)
{
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/logout", soap_endpoint);
    return soap_call_ns1__logout(soap, soap_endpoint, soap_action, session, _param_2); 
}

int searchByModule(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *    
     session, char *search_USCOREstring, struct select_USCOREfields *modules, int offset, int max_USCOREresults, char *assigned_USCOREuser_USCOREid, struct 
     select_USCOREfields *select_USCOREfields, enum xsd__boolean unified_USCOREsearch_USCOREonly, enum xsd__boolean favorites, struct                       
     ns1__search_USCOREby_USCOREmoduleResponse *_param_17)
{    
    return soap_call_ns1__search_USCOREby_USCOREmodule(soap, soap_endpoint, soap_action, session, search_USCOREstring, 
                modules, offset, max_USCOREresults, assigned_USCOREuser_USCOREid, select_USCOREfields, 
                unified_USCOREsearch_USCOREonly, favorites, _param_17); 
}

int getEntryList(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *      
     session, char *module_USCOREname, char *query, char *order_USCOREby, int offset, struct select_USCOREfields *select_USCOREfields, struct               
     link_USCOREnames_USCOREto_USCOREfields_USCOREarray *link_USCOREname_USCOREto_USCOREfields_USCOREarray, int max_USCOREresults, int deleted, enum        
     xsd__boolean favorites, struct ns1__get_USCOREentry_USCORElistResponse *_param_5)
{
    return soap_call_ns1__get_USCOREentry_USCORElist(soap, soap_endpoint, soap_action, session, module_USCOREname, 
                        query, order_USCOREby, offset, select_USCOREfields, link_USCOREname_USCOREto_USCOREfields_USCOREarray, 
                        max_USCOREresults, deleted, favorites, _param_5);
}


int getEntries(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session, char *
     module_USCOREname, struct select_USCOREfields *ids, struct select_USCOREfields *select_USCOREfields, struct                                            
     link_USCOREnames_USCOREto_USCOREfields_USCOREarray *link_USCOREname_USCOREto_USCOREfields_USCOREarray, enum xsd__boolean track_USCOREview, struct      
     ns1__get_USCOREentriesResponse *_param_4)
{
    return soap_call_ns1__get_USCOREentries(soap, soap_endpoint, soap_action, session, module_USCOREname, ids, 
                        select_USCOREfields, link_USCOREname_USCOREto_USCOREfields_USCOREarray, track_USCOREview, _param_4);      
}

int getEntry(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session, char *  
     module_USCOREname, char *id, struct select_USCOREfields *select_USCOREfields, struct link_USCOREnames_USCOREto_USCOREfields_USCOREarray *              
     link_USCOREname_USCOREto_USCOREfields_USCOREarray, enum xsd__boolean track_USCOREview, struct ns1__get_USCOREentryResponse *_param_3)
{
    return soap_call_ns1__get_USCOREentry(soap, soap_endpoint, soap_action, session, module_USCOREname, id, 
                       select_USCOREfields, link_USCOREname_USCOREto_USCOREfields_USCOREarray, track_USCOREview, _param_3);
}

int setEntry(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session, char *  
     module_USCOREname, struct name_USCOREvalue_USCORElist *name_USCOREvalue_USCORElist, struct ns1__set_USCOREentryResponse *_param_9)
{
    return soap_call_ns1__set_USCOREentry(soap, soap_endpoint, soap_action, session, module_USCOREname,  
                        name_USCOREvalue_USCORElist, _param_9);
}

int  setEntries(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session, char *
     module_USCOREname, struct name_USCOREvalue_USCORElists *name_USCOREvalue_USCORElists, struct ns1__set_USCOREentriesResponse *_param_10)
{
    return soap_call_ns1__set_USCOREentries(soap, soap_endpoint, soap_action, session, module_USCOREname, 
                        name_USCOREvalue_USCORElists, _param_10); 
}

int setRelationship(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session,  
     char *module_USCOREname, char *module_USCOREid, char *link_USCOREfield_USCOREname, struct select_USCOREfields *related_USCOREids, struct               
     name_USCOREvalue_USCORElist *name_USCOREvalue_USCORElist, int delete_, struct ns1__set_USCORErelationshipResponse *_param_6)
{
    printf("module_USCOREid = %s\n", module_USCOREid);
    return soap_call_ns1__set_USCORErelationship(soap, soap_endpoint, soap_action,session, module_USCOREname,
                        module_USCOREid, link_USCOREfield_USCOREname, related_USCOREids, name_USCOREvalue_USCORElist, 
                        delete_,  _param_6);
}

int setRelationships(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session,
     struct select_USCOREfields *module_USCOREnames, struct select_USCOREfields *module_USCOREids, struct select_USCOREfields *
     link_USCOREfield_USCOREnames, struct new_USCOREset_USCORErelationhip_USCOREids *related_USCOREids, struct name_USCOREvalue_USCORElists *
     name_USCOREvalue_USCORElists, struct deleted_USCOREarray *delete_USCOREarray, struct ns1__set_USCORErelationshipsResponse *_param_7)
{
 	return soap_call_ns1__set_USCORErelationships(soap, soap_endpoint, soap_action, session, module_USCOREnames,
				module_USCOREids, link_USCOREfield_USCOREnames, related_USCOREids,  name_USCOREvalue_USCORElists, 
				delete_USCOREarray, _param_7);

}

int getAvailableModules(struct soap *soap, const char *soap_endpoint, const char *soap_action,      
     char *session, char *filter, struct ns1__get_USCOREavailable_USCOREmodulesResponse *_param_18)
{
    return soap_call_ns1__get_USCOREavailable_USCOREmodules(soap, soap_endpoint, soap_action, session, filter, _param_18);
}

/* ******** pack call functoin end ****** */

int searchModule(struct soap *soap, const char *soap_endpoint, char *session, enum module_name module_name, char *search_string,
                enum search_type search_type, SGSearchResult *search_result)
{
    struct select_USCOREfields *modules = NULL;
    int offset = 0;
    int max_results = 20;
    char *assigned_user_id = "";//NULL;
    struct select_USCOREfields *select_fields = NULL;
    struct ns1__search_USCOREby_USCOREmoduleResponse *_param_17 = NULL;
    modules = (struct select_USCOREfields *)malloc(sizeof(struct select_USCOREfields ));
    select_fields = (struct select_USCOREfields *)malloc(sizeof(struct select_USCOREfields));
    _param_17 = (struct ns1__search_USCOREby_USCOREmoduleResponse *)malloc(sizeof(struct ns1__search_USCOREby_USCOREmoduleResponse));
    int ret = -1;
    
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/search_by_module", soap_endpoint);
    int item = 0;
	char rel_string[64] = {0};
	char string_number[64] = {0};
    if (CRM_TYPE_PHONE == search_type) 
	{
        //addPercent(rel_string, search_string);
		leave_number(string_number, search_string);
		addPercent(rel_string, string_number);
        printf("rel = %s\n",  rel_string);
		printf("leave = %s\n",  string_number);
    }
	else
	{
		strcpy(rel_string, search_string);
	}
    switch(module_name){
        case CRM_MODULE_ACCOUNTS:
            modules->__ptr = (char**)malloc(sizeof(char*));
            modules->__ptr[0] = strdup("Accounts");
            modules->__size = 1;
            select_fields->__ptr = (char**)malloc(PTR_SIZE);
            select_fields->__ptr[item++] = strdup("id");
            select_fields->__ptr[item++] = strdup("name");
            select_fields->__ptr[item++] = strdup("account_type");
            select_fields->__ptr[item++] = strdup("phone_office");
            select_fields->__ptr[item++] = strdup("assigned_user_name");     
            select_fields->__ptr[item++] = strdup("billing_address_city");
            select_fields->__size = item;

            ret = soap_call_ns1__search_USCOREby_USCOREmodule(soap, soap_endpoint, soap_action, session, rel_string, modules, offset, max_results, "",
                            select_fields, 0, 0, _param_17); 
            break;
        case CRM_MODULE_CONTACTS:
            modules->__ptr = (char**)malloc(sizeof(char*));
            modules->__ptr[0] = strdup("Contacts");
            modules->__size = 1;
            
            select_fields->__ptr = (char**)malloc(PTR_SIZE);
            select_fields->__ptr[item++] = strdup("id");
            select_fields->__ptr[item++] = strdup("name");
            select_fields->__ptr[item++] = strdup("phone_home");
            select_fields->__ptr[item++] = strdup("phone_work");
            select_fields->__ptr[item++] = strdup("phone_mobile");
            select_fields->__ptr[item++] = strdup("assigned_user_name");
            select_fields->__size = item;
            ret = soap_call_ns1__search_USCOREby_USCOREmodule(soap, soap_endpoint, soap_action, session, search_string, modules, offset, max_results, "",
                            select_fields, 0, 0, _param_17); 
            break;
        case CRM_MODULE_LEADS:
            modules->__ptr = (char**)malloc(sizeof(char*));
            modules->__ptr[0] = strdup("Leads");
            modules->__size = 1;
            select_fields->__ptr = (char**)malloc(PTR_SIZE);
            select_fields->__ptr[item++] = strdup("id");
            select_fields->__ptr[item++] = strdup("name");
            select_fields->__ptr[item++] = strdup("phone_home");
            select_fields->__ptr[item++] = strdup("phone_work");
            select_fields->__ptr[item++] = strdup("phone_mobile");
            select_fields->__ptr[item++] = strdup("assigned_user_name");
            select_fields->__size = item;
            ret = soap_call_ns1__search_USCOREby_USCOREmodule(soap, soap_endpoint, soap_action, session, search_string, modules, offset, max_results, "",
                            select_fields, 0, 0, _param_17); 
            break;
        case CRM_MODULE_CALLS:
            modules->__ptr = (char**)malloc(sizeof(char*));
            modules->__ptr[0] = strdup("Calls");
            modules->__size = 1;
            select_fields->__ptr = (char**)malloc(PTR_SIZE);
            select_fields->__ptr[item++] = strdup("id");
            select_fields->__ptr[item++] = strdup("name");
            select_fields->__ptr[item++] = strdup("date_start");
            select_fields->__ptr[item++] = strdup("date_end");
            select_fields->__size = item;
            ret = soap_call_ns1__search_USCOREby_USCOREmodule(soap, soap_endpoint, soap_action, session, search_string, modules, offset, max_results, "",
                            select_fields, 0, 0, _param_17); 
            break;
        default:
            return 0;
            break;
    }

	int f = 0;
	for (f = 0; f < modules->__size; f++) {
		SAFE_FREE(modules->__ptr[f]);
	}
	SAFE_FREE(modules->__ptr);
	SAFE_FREE(modules);
	
		//modules->__ptr[f] = NULL;
	for (f = 0; f < select_fields->__size; f++) {
		SAFE_FREE(select_fields->__ptr[f]);
	}
	SAFE_FREE(select_fields->__ptr);
	SAFE_FREE(select_fields);

    
    if (ret == SOAP_OK) {
        printf("Search Success!\n");
        printf("%d\n", _param_17->return_->entry_USCORElist->__size);
        int value_size = _param_17->return_->entry_USCORElist->__size;
        int i = 0;
        for (i = 0; i < value_size; i++){
            printf("name  = %s\n", _param_17->return_->entry_USCORElist->__ptr[i]->name);
            if (search_result != NULL)
                search_result->module = strdup(_param_17->return_->entry_USCORElist->__ptr[i]->name);
            printf("size = %d\n", _param_17->return_->entry_USCORElist->__ptr[i]->records->__size);
            int records_size = _param_17->return_->entry_USCORElist->__ptr[i]->records->__size;
            for (int j = 0; j < records_size; j++) {
                printf("record[%d] -------------I\n", j);
				char return_id[64] = {0};
				char return_phone_office[64] = {0};
				char return_phone_home[64] = {0};
				char return_phone_work[64] = {0};
				char return_phone_mobile[64] = {0};
                for(int k = 0; k < _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[0]->__size; k++) {
                    printf("    name   = %s\n", _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->name);
                    printf("    value  = %s\n", _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                    if (!strcasecmp("id", _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->name)) {
                        if (search_result != NULL) {
                        	//*result_char = strdup(_param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                        	strcpy(return_id, _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                            //search_result->id = strdup(_param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                        }
                    }
					if (!strcasecmp("phone_office", _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->name)) 
					{
						if (search_result != NULL) {
                        	leave_number(return_phone_office, _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                        }
					}
					if (!strcasecmp("phone_home", _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->name)) 
					{
						if (search_result != NULL) {
                        	leave_number(return_phone_home, _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                        }
					}
					if (!strcasecmp("phone_work", _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->name)) 
					{
						if (search_result != NULL) {
                        	leave_number(return_phone_work, _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                        }
					}
					if (!strcasecmp("phone_mobile", _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->name)) 
					{
						if (search_result != NULL) {
                        	leave_number(return_phone_mobile, _param_17->return_->entry_USCORElist->__ptr[i]->records->__ptr[j]->__ptr[k]->value);
                        }
					}
					if (!strcmp(string_number, return_phone_office) || 
						!strcmp(string_number, return_phone_home) ||
						!strcmp(string_number, return_phone_work) ||
						!strcmp(string_number, return_phone_mobile))
					{
						search_result->id = strdup(return_id);
						SAFE_FREE(_param_17);
						return ret;
					}
                }
            }
        }
		SAFE_FREE(_param_17);
        return ret;
    } else if (ret == SOAP_TYPE) {
        printf("Result not match!\n");
        if (_param_17 == NULL)
            printf("17 is null\n");
        else
            printf("17 is not null\n");
		SAFE_FREE(_param_17);
        return ret;
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
		SAFE_FREE(_param_17);
        return ret;
    }
}

int setCall(struct soap *soap, const char *soap_endpoint, char *session, char *call_id, SGCallItem *call_item, char **return_id)
{
    /* call_id if NULL, added new , if not NULL, update*/
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_entry", soap_endpoint);
    
    struct name_USCOREvalue_USCORElist *setEntry_name_value_list = NULL;
    struct ns1__set_USCOREentryResponse *_param_9 = NULL;
    setEntry_name_value_list = (struct name_USCOREvalue_USCORElist *)malloc(sizeof(struct name_USCOREvalue_USCORElist));
    _param_9 = (struct ns1__set_USCOREentryResponse *)malloc(sizeof(struct ns1__set_USCOREentryResponse));
    int item = 0;
    setEntry_name_value_list->__ptr = (struct ns1__name_USCOREvalue**)malloc(PTR_SIZE);
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("name");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->name ? call_item->name : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("parent_type");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->parent_type ? call_item->parent_type : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("parent_id");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->parent_id ? call_item->parent_id : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("direction");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->direction ? call_item->direction : "");  
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("description");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->description ? call_item->description : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("duration_minutes");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->duration_minutes ? call_item->duration_minutes : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("duration_hours");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->duration_hours ? call_item->duration_hours : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("date_start");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->date_start ? call_item->date_start : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("assigned_user_id");
    setEntry_name_value_list->__ptr[item++]->value = strdup(call_item->assigned_user_id ? call_item->assigned_user_id : "");
    if (call_id != NULL && strlen(call_id) > 0) {
        setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntry_name_value_list->__ptr[item]->name = strdup("id");
        setEntry_name_value_list->__ptr[item++]->value = strdup(call_id);
    } 
    setEntry_name_value_list->__size = item;
    
    int ret = setEntry(soap, soap_endpoint, soap_action, session, "Calls", setEntry_name_value_list, _param_9);
        printf("8*****\n");
    if (ret == SOAP_OK) {
        printf("Success setEntry!\n");
        //snprintf(set_entry_res_id, sizeof(set_entry_res_id), "%s\0\n", _param_9->return_->id);
        *return_id = strdup(_param_9->return_->id);
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
        //goto end;
    }
    int j = 0;
	for(j = 0; j < item; j++)
	{
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->name);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->value);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]);
	}
	SAFE_FREE(setEntry_name_value_list->__ptr);
	SAFE_FREE(setEntry_name_value_list);
    SAFE_FREE(_param_9);
    return ret;
}

int setCalls(struct soap *soap, const char *soap_endpoint, char *session, SGCallsItem *calls_item, SGReturnCallsID *return_calls_id)
{
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_entries", soap_endpoint);
    
    //struct name_USCOREvalue_USCORElist *setEntry_name_value_list = NULL;
    struct name_USCOREvalue_USCORElists *setEntries_name_value_lists = NULL;
    //struct ns1__set_USCOREentryResponse *_param_9 = NULL;
    struct ns1__set_USCOREentriesResponse *_param_10 = NULL;
    setEntries_name_value_lists = (struct name_USCOREvalue_USCORElists *)malloc(sizeof(struct name_USCOREvalue_USCORElists));
    //setEntry_name_value_list = (struct name_USCOREvalue_USCORElist *)soap_malloc(soap, sizeof(struct name_USCOREvalue_USCORElist));
    _param_10 = (struct ns1__set_USCOREentriesResponse *)malloc(sizeof(struct ns1__set_USCOREentriesResponse));
	setEntries_name_value_lists->__ptr = (struct name_USCOREvalue_USCORElist **)malloc(PTR_SIZE * PTR_SIZE);
    SGCallsItem *calls_tmp = calls_item;
    //struct call_item *tmp = NULL;
    //for (calls_tmp = NULL; tmp = calls_tmp->call_item; tmp = calls_tmp)
    int items = 0;
	int item = 0;
    while(calls_tmp) {
        calls_tmp = calls_tmp->next;
        item = 0;
        setEntries_name_value_lists->__ptr[items]->__ptr = (struct ns1__name_USCOREvalue**)malloc(PTR_SIZE);
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("name");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->name ? calls_tmp->call_item->name : "");
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("parent_type");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->parent_type ? calls_tmp->call_item->parent_type : "");
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("parent_id");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->parent_id ? calls_tmp->call_item->parent_id : "");
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("direction");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->direction ? calls_tmp->call_item->direction : "");  
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("description");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->description ? calls_tmp->call_item->description : "");
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("duration_minutes");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->duration_minutes ? calls_tmp->call_item->duration_minutes : "");
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("duration_hours");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->duration_hours ? calls_tmp->call_item->duration_hours : "");
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("date_start");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->date_start ? calls_tmp->call_item->date_start : "");
        setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("assigned_user_id");
        setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_item->assigned_user_id ? calls_tmp->call_item->assigned_user_id : "");
        if (calls_tmp->call_id != NULL && strlen(calls_tmp->call_id) > 0) {
            setEntries_name_value_lists->__ptr[items]->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
            setEntries_name_value_lists->__ptr[items]->__ptr[item]->name = strdup("id");
            setEntries_name_value_lists->__ptr[items]->__ptr[item++]->value = strdup(calls_tmp->call_id);
        } 
        setEntries_name_value_lists->__ptr[items]->__size = item;
        items++;
    }
    
    
    int ret = setEntries(soap, soap_endpoint, soap_action, session, "Calls", setEntries_name_value_lists, _param_10);
    if (ret == SOAP_OK) {
        printf("Success setEntry!\n");
        //snprintf(set_entry_res_id, sizeof(set_entry_res_id), "%s\0\n", _param_9->return_->id);
        //*return_id = strdup(_param_10->return_->ids->__ptr);
        SGReturnCallsID *tmp_return_calls_id = return_calls_id;
        for (int i = 0; i < _param_10->return_->ids->__size; i++) {
            printf("_param_10->return_->ids->__ptr = %s\n", _param_10->return_->ids->__ptr[i]);
			tmp_return_calls_id = initSGReturnCallsID(tmp_return_calls_id);
			tmp_return_calls_id->return_call_id = strdup(_param_10->return_->ids->__ptr[i]);
			tmp_return_calls_id = tmp_return_calls_id->next;
        }
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
        //goto end;
    }
    int j = 0, k = 0;
	for (j = 0; j < items; j++)
	{
		for(k = 0; k < item; k++)
		{
			SAFE_FREE(setEntries_name_value_lists->__ptr[j]->__ptr[k]->name);
			SAFE_FREE(setEntries_name_value_lists->__ptr[j]->__ptr[k]->value);
			SAFE_FREE(setEntries_name_value_lists->__ptr[j]->__ptr[k]);
		}
		SAFE_FREE(setEntries_name_value_lists->__ptr[j]->__ptr);
		SAFE_FREE(setEntries_name_value_lists->__ptr[j]);
	}
	SAFE_FREE(setEntries_name_value_lists->__ptr);
	SAFE_FREE(setEntries_name_value_lists);
	SAFE_FREE(_param_10);
    return ret;
}

int setCallRelation(struct soap *soap, const char *soap_endpoint, char *session, char *call_id, char *link_name, char *link_id, SGLinkReturn *link_return)
{
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_relationship", soap_endpoint);
    
    struct ns1__set_USCORErelationshipResponse *_param_6 = NULL;
    _param_6 = (struct ns1__set_USCORErelationshipResponse *)soap_malloc(soap, sizeof(struct ns1__set_USCORErelationshipResponse));
    
    struct select_USCOREfields *related_USCOREids = NULL;
    related_USCOREids = (struct select_USCOREfields *)soap_malloc(soap, sizeof(struct select_USCOREfields));
    related_USCOREids->__ptr = (char**)soap_malloc(soap, PTR_SIZE);
    related_USCOREids->__ptr[0] = strdup(link_id);
    related_USCOREids->__size = 1;
    
    int delete_ = 0;
    struct name_USCOREvalue_USCORElist *rel_name_value_list = NULL;
    /*rel_name_value_list = (struct name_USCOREvalue_USCORElist *)soap_malloc(soap, sizeof(struct name_USCOREvalue_USCORElist));
    rel_name_value_list->__ptr = (struct ns1__name_USCOREvalue**)soap_malloc(soap, PTR_SIZE);
    rel_name_value_list->__ptr[0] = (struct ns1__name_USCOREvalue*)soap_malloc(soap, sizeof(struct ns1__name_USCOREvalue));
    rel_name_value_list->__ptr[0]->name = strdup("contact_role");
    rel_name_value_list->__ptr[0]->value = strdup("Other");
    rel_name_value_list->__size = 1; */

    int ret = setRelationship(soap, soap_endpoint, soap_action, session, "Calls", call_id, link_name, related_USCOREids, 
                rel_name_value_list, delete_, _param_6);
    if (ret == SOAP_OK) {
        printf("Success setRelationship!\n");
        link_return->created = _param_6->return_->created;
        link_return->failed = _param_6->return_->failed;
        link_return->deleted = _param_6->return_->deleted;
        //printf("created = %d\nfailed = %d\ndeleted = %d\n", 
        //        _param_6->return_->created, _param_6->return_->failed, _param_6->return_->deleted);
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
        //goto end;
    }
	int j = 0;
	for (j = 0; j < related_USCOREids->__size; j++)
	{
		SAFE_FREE(related_USCOREids->__ptr[j]);
	}
	SAFE_FREE(related_USCOREids->__ptr);
	SAFE_FREE(related_USCOREids);
	SAFE_FREE(_param_6);
    return ret;
}

int setContactRelation(struct soap *soap, const char *soap_endpoint, char *session, char *contact_id, char *link_name, char *link_id, SGLinkReturn *link_return)
{
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_relationship", soap_endpoint);
    
    struct ns1__set_USCORErelationshipResponse *_param_6 = NULL;
    _param_6 = (struct ns1__set_USCORErelationshipResponse *)malloc(sizeof(struct ns1__set_USCORErelationshipResponse));
    
    struct select_USCOREfields *related_USCOREids = NULL;
    related_USCOREids = (struct select_USCOREfields *)malloc(sizeof(struct select_USCOREfields));
    related_USCOREids->__ptr = (char**)malloc(PTR_SIZE);
    related_USCOREids->__ptr[0] = strdup(link_id);
    related_USCOREids->__size = 1;
    
    int delete_ = 0;
    struct name_USCOREvalue_USCORElist *rel_name_value_list = NULL;
    /*rel_name_value_list = (struct name_USCOREvalue_USCORElist *)soap_malloc(soap, sizeof(struct name_USCOREvalue_USCORElist));
    rel_name_value_list->__ptr = (struct ns1__name_USCOREvalue**)soap_malloc(soap, PTR_SIZE);
    rel_name_value_list->__ptr[0] = (struct ns1__name_USCOREvalue*)soap_malloc(soap, sizeof(struct ns1__name_USCOREvalue));
    rel_name_value_list->__ptr[0]->name = strdup("contact_role");
    rel_name_value_list->__ptr[0]->value = strdup("Other");
    rel_name_value_list->__size = 1; */

    int ret = setRelationship(soap, soap_endpoint, soap_action, session, "Contacts", contact_id, link_name, related_USCOREids, 
                rel_name_value_list, delete_, _param_6);
    if (ret == SOAP_OK) {
        printf("Success setContactRelation!\n");
        link_return->created = _param_6->return_->created;
        link_return->failed = _param_6->return_->failed;
        link_return->deleted = _param_6->return_->deleted;
        //printf("created = %d\nfailed = %d\ndeleted = %d\n", 
        //        _param_6->return_->created, _param_6->return_->failed, _param_6->return_->deleted);
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
        //goto end;
    }
	int j = 0;
	for (j = 0; j < related_USCOREids->__size; j++)
	{
		SAFE_FREE(related_USCOREids->__ptr[j]);
	}
	SAFE_FREE(related_USCOREids->__ptr);
	SAFE_FREE(related_USCOREids);
	SAFE_FREE(_param_6);
    return ret;
}

int setContactRelations(struct soap * soap,const char * soap_endpoint,char * session, SGContactsID *contacts_id, 
								char *link_name, SGLinksID *links_id, int num, SGLinkReturn *link_return)
{
	char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_relationships", soap_endpoint);
	struct select_USCOREfields *module_USCOREnames = NULL;
	struct select_USCOREfields *module_USCOREids = NULL;
	struct select_USCOREfields *link_USCOREfield_USCOREnames = NULL;
	struct new_USCOREset_USCORErelationhip_USCOREids *related_USCOREids = NULL;
	struct deleted_USCOREarray *delete_USCOREarray = NULL;
	SGContactsID *tmp_contacts_id = contacts_id;
	SGLinksID *tmp_links_id = links_id;
	module_USCOREnames = (struct select_USCOREfields *)malloc(sizeof(struct select_USCOREfields));
	module_USCOREids = (struct select_USCOREfields *)malloc(sizeof(struct select_USCOREfields));
	link_USCOREfield_USCOREnames = (struct select_USCOREfields *)malloc(sizeof(struct select_USCOREfields));
	related_USCOREids = (struct new_USCOREset_USCORErelationhip_USCOREids *)malloc(sizeof(struct new_USCOREset_USCORErelationhip_USCOREids));
	module_USCOREnames->__ptr =  (char **)malloc(PTR_SIZE * PTR_SIZE);
	module_USCOREids->__ptr = (char **)malloc(PTR_SIZE * PTR_SIZE);
	link_USCOREfield_USCOREnames->__ptr = (char **)malloc(PTR_SIZE * PTR_SIZE);
	related_USCOREids->__ptr = (struct select_USCOREfields **)malloc(sizeof(struct select_USCOREfields*));

	//related_USCOREids->__size = 1;
	
	int i = 0;

	for (i = 0; i < num; i++)
	{
		if (tmp_contacts_id && tmp_contacts_id->id &&
			tmp_links_id && tmp_links_id->id)
		{
			module_USCOREnames->__ptr[i] = strdup("Contacts");

			link_USCOREfield_USCOREnames->__ptr[i] = strdup(link_name);

			module_USCOREids->__ptr[i] = strdup(tmp_contacts_id->id);

			related_USCOREids->__ptr[i] = (struct select_USCOREfields *)malloc(sizeof(struct select_USCOREfields));
			related_USCOREids->__ptr[i]->__ptr = (char **)malloc(PTR_SIZE * PTR_SIZE);
			related_USCOREids->__ptr[i]->__ptr[0] = strdup(tmp_links_id->id);

			module_USCOREnames->__size = i + 1;
			link_USCOREfield_USCOREnames->__size = i + 1;
			module_USCOREids->__size = i + 1;
			related_USCOREids->__size = i + 1;
			related_USCOREids->__ptr[i]->__size = 1;
			tmp_contacts_id = tmp_contacts_id->next;
			tmp_links_id = tmp_links_id->next;
		}
	}

	struct ns1__set_USCORErelationshipsResponse *_param_7 = NULL;
	_param_7 = (struct ns1__set_USCORErelationshipsResponse *)malloc(sizeof(struct ns1__set_USCORErelationshipsResponse));
	int ret = setRelationships(soap, soap_endpoint, soap_action, session, module_USCOREnames,
				module_USCOREids, link_USCOREfield_USCOREnames, related_USCOREids,  NULL, 
				delete_USCOREarray, _param_7);
	if (ret == SOAP_OK)
	{
        printf("setContactRelations Success!!!!\n");
	}
	int j = 0;
	for (j = 0; j < num; j++)
	{
		SAFE_FREE(module_USCOREnames->__ptr[j]);
		SAFE_FREE(link_USCOREfield_USCOREnames->__ptr[j]);
		SAFE_FREE(module_USCOREids->__ptr[j]);
		SAFE_FREE(related_USCOREids->__ptr[j]->__ptr[0]);
		SAFE_FREE(related_USCOREids->__ptr[j]->__ptr);
		SAFE_FREE(related_USCOREids->__ptr[j]);
	}
	SAFE_FREE(related_USCOREids->__ptr);
	SAFE_FREE(related_USCOREids);
	SAFE_FREE(link_USCOREfield_USCOREnames->__ptr);
	SAFE_FREE(link_USCOREfield_USCOREnames);
	SAFE_FREE(module_USCOREids->__ptr);
	SAFE_FREE(module_USCOREids);
	SAFE_FREE(module_USCOREnames->__ptr);
	SAFE_FREE(module_USCOREnames);
	SAFE_FREE(_param_7);
	return ret;
}

int setContact(struct soap *soap, const char *soap_endpoint, char *session, char *contact_id, SGContactItem *contact_item, char **return_id)
{
	/* contact_id if NULL, added new , if not NULL, update*/
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_entry", soap_endpoint);
	
    struct name_USCOREvalue_USCORElist *setEntry_name_value_list = NULL;
    struct ns1__set_USCOREentryResponse *_param_9 = NULL;
    setEntry_name_value_list = (struct name_USCOREvalue_USCORElist *)malloc(sizeof(struct name_USCOREvalue_USCORElist));
    _param_9 = (struct ns1__set_USCOREentryResponse *)malloc(sizeof(struct ns1__set_USCOREentryResponse));
    int item = 0;
    setEntry_name_value_list->__ptr = (struct ns1__name_USCOREvalue**)malloc(PTR_SIZE);
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("title");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->title ? contact_item->title : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("first_name");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->first_name ? contact_item->first_name : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("last_name");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->last_name ? contact_item->last_name : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_home");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->phone_home ? contact_item->phone_home : "");  
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_mobile");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->phone_mobile ? contact_item->phone_mobile : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_work");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->phone_work ? contact_item->phone_work : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_other");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->phone_other ? contact_item->phone_other : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_fax");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->phone_fax ? contact_item->phone_fax : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("description");
    setEntry_name_value_list->__ptr[item++]->value = strdup(contact_item->description ? contact_item->description : "");
    if (contact_id != NULL && strlen(contact_id) > 0) {
        setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntry_name_value_list->__ptr[item]->name = strdup("id");
        setEntry_name_value_list->__ptr[item++]->value = strdup(contact_id);
    } 
    setEntry_name_value_list->__size = item;
    
    int ret = setEntry(soap, soap_endpoint, soap_action, session, "Contacts", setEntry_name_value_list, _param_9);
        printf("8*****\n");
    if (ret == SOAP_OK) {
        printf("Success setContact!\n");
        //snprintf(set_entry_res_id, sizeof(set_entry_res_id), "%s\0\n", _param_9->return_->id);
        *return_id = strdup(_param_9->return_->id);
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
        //goto end;
    }
    int j = 0;
	for(j = 0; j < item; j++)
	{
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->name);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->value);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]);
	}
	SAFE_FREE(setEntry_name_value_list->__ptr);
	SAFE_FREE(setEntry_name_value_list);
    SAFE_FREE(_param_9);
    return ret;
}

int setAccount(struct soap *soap, const char *soap_endpoint, char *session, char *account_id, SGAccountItem *account_item, char **return_id)
{
	/* contact_id if NULL, added new , if not NULL, update*/
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_entry", soap_endpoint);
	
    struct name_USCOREvalue_USCORElist *setEntry_name_value_list = NULL;
    struct ns1__set_USCOREentryResponse *_param_9 = NULL;
    setEntry_name_value_list = (struct name_USCOREvalue_USCORElist *)malloc(sizeof(struct name_USCOREvalue_USCORElist));
    _param_9 = (struct ns1__set_USCOREentryResponse *)malloc(sizeof(struct ns1__set_USCOREentryResponse));
    int item = 0;
    setEntry_name_value_list->__ptr = (struct ns1__name_USCOREvalue**)malloc(PTR_SIZE);
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("name");
    setEntry_name_value_list->__ptr[item++]->value = strdup(account_item->name ? account_item->name : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_office");
    setEntry_name_value_list->__ptr[item++]->value = strdup(account_item->phone_office ? account_item->phone_office : "");  
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_alternate");
    setEntry_name_value_list->__ptr[item++]->value = strdup(account_item->phone_alternate ? account_item->phone_alternate : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_fax");
    setEntry_name_value_list->__ptr[item++]->value = strdup(account_item->phone_fax ? account_item->phone_fax : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("description");
    setEntry_name_value_list->__ptr[item++]->value = strdup(account_item->description ? account_item->description : "");
    if (account_id != NULL && strlen(account_id) > 0) {
        setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntry_name_value_list->__ptr[item]->name = strdup("id");
        setEntry_name_value_list->__ptr[item++]->value = strdup(account_id);
    } 
    setEntry_name_value_list->__size = item;
    
    int ret = setEntry(soap, soap_endpoint, soap_action, session, "Accounts", setEntry_name_value_list, _param_9);
        printf("8*****\n");
	
    if (ret == SOAP_OK) {
        printf("Success setContact!\n");
        //snprintf(set_entry_res_id, sizeof(set_entry_res_id), "%s\0\n", _param_9->return_->id);
        *return_id = strdup(_param_9->return_->id);
		
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
        //goto end;
    }
	int j = 0;
	for(j = 0; j < item; j++)
	{
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->name);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->value);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]);
	}
	SAFE_FREE(setEntry_name_value_list->__ptr);
	SAFE_FREE(setEntry_name_value_list);
    SAFE_FREE(_param_9);
    return ret;
}

int setLead(struct soap *soap, const char *soap_endpoint, char *session, char *lead_id, SGLeadItem *lead_item, char **return_id)
{
	/* lead_id if NULL, added new , if not NULL, update*/
    char soap_action[128] = {0};
    memset(soap_action, 0, sizeof(soap_action));
    snprintf(soap_action, sizeof(soap_action), "%s/set_entry", soap_endpoint);
	
    struct name_USCOREvalue_USCORElist *setEntry_name_value_list = NULL;
    struct ns1__set_USCOREentryResponse *_param_9 = NULL;
    setEntry_name_value_list = (struct name_USCOREvalue_USCORElist *)malloc(sizeof(struct name_USCOREvalue_USCORElist));
    _param_9 = (struct ns1__set_USCOREentryResponse *)malloc(sizeof(struct ns1__set_USCOREentryResponse));
    int item = 0;
    setEntry_name_value_list->__ptr = (struct ns1__name_USCOREvalue**)malloc(PTR_SIZE);
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("title");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->title ? lead_item->title : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("first_name");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->first_name ? lead_item->first_name : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("last_name");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->last_name ? lead_item->last_name : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_home");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->phone_home ? lead_item->phone_home : "");  
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_mobile");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->phone_mobile ? lead_item->phone_mobile : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_work");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->phone_work ? lead_item->phone_work : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_other");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->phone_other ? lead_item->phone_other : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("phone_fax");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->phone_fax ? lead_item->phone_fax : "");
    setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
    setEntry_name_value_list->__ptr[item]->name = strdup("description");
    setEntry_name_value_list->__ptr[item++]->value = strdup(lead_item->description ? lead_item->description : "");
    if (lead_id != NULL && strlen(lead_id) > 0) {
        setEntry_name_value_list->__ptr[item] = (struct ns1__name_USCOREvalue*)malloc(sizeof(struct ns1__name_USCOREvalue));
        setEntry_name_value_list->__ptr[item]->name = strdup("id");
        setEntry_name_value_list->__ptr[item++]->value = strdup(lead_id);
    } 
    setEntry_name_value_list->__size = item;
    
    int ret = setEntry(soap, soap_endpoint, soap_action, session, "Contacts", setEntry_name_value_list, _param_9);
        printf("8*****\n");
    if (ret == SOAP_OK) {
        printf("Success setContact!\n");
        //snprintf(set_entry_res_id, sizeof(set_entry_res_id), "%s\0\n", _param_9->return_->id);
        *return_id = strdup(_param_9->return_->id);
    } else {
        printf("ret = %d\n", ret);
        soap_print_fault(soap, stderr);
        //goto end;
    }
    int j = 0;
	for(j = 0; j < item; j++)
	{
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->name);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]->value);
		SAFE_FREE(setEntry_name_value_list->__ptr[j]);
	}
	SAFE_FREE(setEntry_name_value_list->__ptr);
	SAFE_FREE(setEntry_name_value_list);
    SAFE_FREE(_param_9);
    return ret;
}



int SGCRMLogin(SGServerInfo *server_info, char *username, char *password, SGLoginReturn *login_return)
{
    int ret = -1;
    struct ns1__loginResponse *_param_1;
    _param_1 = (struct ns1__loginResponse*)soap_malloc(server_info->soap, sizeof(struct ns1__entry_USCOREvalue));
    ret = sugarLogin(server_info->soap, server_info->soap_endpoint, username, password, _param_1);
    if (ret == SOAP_OK) {
        printf("CRMLogin Success!\n");
        if (login_return) {
            login_return->session_id = strdup(_param_1->return_->id);
        }

        int value_size = _param_1->return_->name_USCOREvalue_USCORElist->__size;
        int i = 0;
        for (i = 0; i < value_size; i++){
            if (!strcmp(_param_1->return_->name_USCOREvalue_USCORElist->__ptr[i]->name, "user_id") && login_return) {
                login_return->user_id = strdup(_param_1->return_->name_USCOREvalue_USCORElist->__ptr[i]->value);
            }
        }
    } else {
        soap_print_fault(server_info->soap, stderr);
    }
    return ret;
}

int SGCRMLogout(SGServerInfo *server_info, char *session)
{
    return sugarLogout(server_info->soap, server_info->soap_endpoint, session, NULL);
}

int updateCall(SGServerInfo *server_info, char *session, char *call_id, SGCallItem *call_item, char **return_call_id)
{
    int ret = -1;
    ret = setCall(server_info->soap, server_info->soap_endpoint, session, call_id, call_item, return_call_id);
	return ret;
}

int addCall(SGServerInfo *server_info, char *session, SGCallItem *call_item, char **return_call_id)
{
    int ret = -1;
    ret = setCall(server_info->soap, server_info->soap_endpoint, session, NULL, call_item, return_call_id);
	return ret;
}

int updateCallBatch(SGServerInfo *server_info, char *session, SGCallsItem *calls_item, SGReturnCallsID *return_calls_id)
{
    int ret = -1;
    ret = setCalls(server_info->soap, server_info->soap_endpoint, session, calls_item, return_calls_id);
	return ret;
}

int addCallBatch(SGServerInfo *server_info, char *session, SGCallsItem *calls_item, SGReturnCallsID *return_calls_id)
{
    int ret = -1;
    ret = setCalls(server_info->soap, server_info->soap_endpoint, session, calls_item, return_calls_id);
	return ret;
}

int addContact(SGServerInfo *server_info, char *session, SGContactItem *contact_item, char **return_contact_id)
{
	int ret = -1;
	ret = setContact(server_info->soap, server_info->soap_endpoint, session, NULL, contact_item, return_contact_id);
	return ret;
}

int addAccount(SGServerInfo *server_info, char *session, SGAccountItem *account_item, char **return_account_id)
{
	int ret = -1;
	ret = setAccount(server_info->soap, server_info->soap_endpoint, session, NULL, account_item, return_account_id);
	return ret;
}

int addLead(SGServerInfo *server_info, char *session, SGLeadItem *lead_item, char **return_account_id)
{
	int ret = -1;
	ret = setLead(server_info->soap, server_info->soap_endpoint, session, NULL, lead_item, return_account_id);
	return ret;
}


int searchAccounts(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result)
{
    int ret = -1;
    ret = searchModule(server_info->soap, server_info->soap_endpoint, session, CRM_MODULE_ACCOUNTS, search_string,
                CRM_TYPE_PHONE, search_result);
	return ret;
}

int searchContacts(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result)
{
    int ret = -1;
    ret = searchModule(server_info->soap, server_info->soap_endpoint, session, CRM_MODULE_CONTACTS, search_string,
                CRM_TYPE_PHONE, search_result);
	return ret;
}

int searchLeads(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result)
{
    int ret = -1;
    ret = searchModule(server_info->soap, server_info->soap_endpoint, session, CRM_MODULE_LEADS, search_string,
                CRM_TYPE_PHONE, search_result);
	return ret;
}

int searchCalls(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result)
{
    int ret = -1;
    ret = searchModule(server_info->soap, server_info->soap_endpoint, session, CRM_MODULE_CALLS, search_string,
                CRM_TYPE_PHONE, search_result);
	return ret;
}

int callLinkUser(SGServerInfo *server_info, char *session, char *call_id, char *user_id, SGLinkReturn *link_return)
{
    int ret = -1;
    ret = setCallRelation(server_info->soap, server_info->soap_endpoint, session, call_id, "users", user_id, link_return);
	return ret;
}

int callLinkContact(SGServerInfo *server_info, char *session, char *call_id, char *contact_id, SGLinkReturn *link_return)
{
    int ret = -1;
    ret = setCallRelation(server_info->soap, server_info->soap_endpoint, session, call_id, "contacts", contact_id, link_return);
	return ret;
}

int callLinkLead(SGServerInfo *server_info, char *session, char *call_id, char *lead_id, SGLinkReturn *link_return)
{
    int ret = -1;
    ret = setCallRelation(server_info->soap, server_info->soap_endpoint, session, call_id, "leads", lead_id, link_return);
	return ret;
}

int contactLinkCall(SGServerInfo *server_info, char *session, char *contact_id, char * call_id, SGLinkReturn * link_return)
{
	int ret = -1;
	ret = setContactRelation(server_info->soap, server_info->soap_endpoint, session, contact_id,  "calls", call_id, link_return);
	return ret;
}

int contactsLinkCalls(SGServerInfo *server_info, char *session, SGContactsID *contacts_id, SGLinksID *links_id, int num, SGLinkReturn *link_return)
{
	int ret = -1;
	ret = setContactRelations(server_info->soap, server_info->soap_endpoint, session, contacts_id, 
							"calls", links_id, num, link_return);
	return ret;
}

