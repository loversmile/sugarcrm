/*************************************************************************
	> File Name: clientSugar.c
	> Author: jklou
	> Mail: 
	> Created Time: Fri 13 May 2016 10:52:31 PM PDT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "sugar_global.h"
int g_crm_task;


Queue request_crm_queue = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, 0};

int addSGCRMTask(SGCRMTask *crm_task)
{
    if (crm_task == NULL)
    {
        return -1;
    }
    int ret = enqueueTask(&request_crm_queue, crm_task);
    printf("en ret = %d\n", ret);
    return 0;
}

int monitorTaskAll()
{
	/* a function need to build calls */
	printf("nonitorajdfkl ajsdlk fjadsf \n");
    SGCRMTask *crm_task = NULL;
    crm_task = initSGCRMTask(crm_task);
    crm_task->search_string = strdup("(586) 4-4536");
    addSGCRMTask(crm_task);
}

int executeRequest(SGCRMInfoTask *info_task, SGCRMTask *crm_task)
{
	
	if (crm_task->search_result == NULL)
	{
		crm_task->search_result = initSGSearchResult(crm_task->search_result);
	}
	int res = searchAccounts(info_task->server_info, info_task->login_return->session_id, crm_task->search_string, crm_task->search_result);
	if (res == SOAP_OK)
	{
		if (crm_task->search_result->id == NULL)
		{
			printf("Not search ,continue need add\n");
			/* choose added to Account,Contacts,Leads */
			//continue;
		}
		printf("id = %s\nmodule = %s\n", crm_task->search_result->id, crm_task->search_result->module);
	}
	/*
    execute();
    search();
    cdr();
    addCall();
    callLinkUser();
    callLinkContact();*/
    return res;
}
void *crmClient(void *data)
{
    char *server = "http://192.168.124.27/crm/service/v4_1/soap.php";
    //char *server = "http://192.168.124.99/crm/service/v4_1/soap.php";
    
    //struct login_return = NULL;
    //login_return = initLoginReturn(login_return);
    int normal = 1;
    int time_now = 0;
    
    SGCRMInfoTask *info_task = NULL;
    info_task = initSGCRMInfoTask(info_task);
    //info_task->server_info = initSGServerInfo(info_task->server_info);
    info_task->server_info->soap = soap_new();
    info_task->server_info->soap_endpoint = strdup(server);
    
    queueInitialize(&request_crm_queue);
    
    SGCRMTask *crm_task = NULL;
    
    char *username = "admin";
    char *password = "123456";
    int login_ret = -1;
    int ret = -1;
    while(g_crm_task) {
        printf("%d\n", time(NULL));
        printf("%d\n", time_now);
        //This need a function to monitor crmtask
		monitorTaskAll();

        if (normal == 1 && (time(NULL) - time_now) >= 1790 )
        {
            time_now = time(NULL);
            //info_task->login_return = initSGLoginReturn(info_task->login_return);
            login_ret = SGCRMLogin(info_task->server_info, username, password, info_task->login_return); 
        } 
        else if (normal == 0)
        {
            //info_task->login_return = initSGLoginReturn(info_task->login_return);
            login_ret = SGCRMLogin(info_task->server_info, username, password, info_task->login_return);
        }

        if (login_ret == 0 && info_task->login_return && 
                info_task->login_return->session_id && 
                info_task->login_return->user_id)
        {
            printf("session = %s\n", info_task->login_return->session_id);
            printf("user    = %s\n", info_task->login_return->user_id);
            normal = 1;
        } 
        else 
        {
            printf("login error login_ret = %d\n", login_ret);
            normal = 0;
            sleep(1);
            continue;
        }
        
        while(DEQUEUE_SUCCESS_AND_CONTINUE == (ret = dequeueTask(&request_crm_queue, &crm_task)) || DEQUEUE_SUCCESS_AND_EMPTY == ret)
        {
            if (crm_task)
            {
                int res = executeRequest(info_task, crm_task);
                
            }
        }
        cleanSGCRMTask(crm_task);
        sleep(1);
		//break;
    }
	cleanSGCRMInfoTask(info_task);
    cleanQueue(&request_crm_queue);
}

void crmClientStart()
{
    g_crm_task = 1;
    pthread_t crm_client_phread;
    pthread_attr_t crm_request_attr;
    pthread_attr_init(&crm_request_attr);
    pthread_attr_setstacksize((&crm_request_attr), CRM_STACK_SIZE);
    pthread_attr_setdetachstate((&crm_request_attr), PTHREAD_CREATE_DETACHED); //PTHREAD_CREATE_DETACHED
    //connectConfigDB(DB_CONF_CCRM);
    //connectConfigDB(DB_CDR_CCRM);
    //connectConfigDB(DB_STATUS_CCRM);
    pthread_create(&crm_client_phread, &crm_request_attr, (void *)crmClient, NULL);

}

int main(int argc, char *argv[])
{
	crmClientStart();
	while(1)
    {
        sleep(1);
    }
}
