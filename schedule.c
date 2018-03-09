// �ȿ� SLS(Simplified List Scheduling) �㷨
// SLS �㷨 : ���ȼ���������������Ȩֵ BL(BL/TL), ���Ŷ��������������Ȩֵ�������ȼ�����,����ٸ������ȼ��ߵ����ε��ȸ�������.
// ��ε��ȵ�������: ���񱻵������ܹ����翪ʼִ�еĴ���Ԫ��.
// 

// int debug=0;

////////////////////////////////////////////////////////////////////////////////////////////////
// �������ÿ�ʼ

#define  TEST_MODE				0  		// ѡ�����ģʽ����ͳ��ģʽ

#define  TaskGraphType 			2		// �ɸ� ȡֵ 1,2,3,4,5,6 // ����ģʽ������νȡֵ
#define  normalized_scale		3 		// �ɸ� ����������Ŀ, ȡֵ 1,2,3,4,5

#define	 NP						4		// �ɸ� // ����Ԫ����Ǵ�1��NP // �������Ǵ�0��(NT-1)
#define  CCR_Grade				1		// �ɸ� ����ͨ�ż����ֵ // -2:1/9, -1:1/3, 0:1, +1:1*3, +2:1*9 // CCR������ֵ���ֵ������
#define  MAX_BLOCK_TASK_NUMBER_FOR_OC  6// �ɸ� ����OC�㷨���


// �������ý���
////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// ��������ͼ����,����Ķ� 
#define  Input					1
#define  Output					2
#define  MeanValue				3
#define  FFT					4
#define  ForkJoin				5
#define  GaussianElimination	6

///////////////////////////////////

#if(TEST_MODE==1)
	#define  SAMPLE_NUMBER		1    // ����Ķ�
#else
	#define  SAMPLE_NUMBER		500  // �ɸ� ������Ŀ
#endif

///////////////////////////////////
// ����Ķ� 
#if(TEST_MODE==1)
	#define  NT 	12
	#define  NC		14
	
#elif(TaskGraphType==Input)
	#define  Scale  (normalized_scale+2)
	#define  NT		( (1<<Scale) - 1 )
	#define  NC		(NT-1)
	
#elif(TaskGraphType==Output)
	#define  Scale  (normalized_scale+2)
	#define  NT		( (1<<Scale) - 1 )
	#define  NC		(NT-1)
	
#elif(TaskGraphType==MeanValue)

	#if(normalized_scale==1)
		#define Scale 3
	#elif(normalized_scale==2)
		#define Scale 4
	#elif(normalized_scale==3)
		#define Scale 6
	#elif(normalized_scale==4)
		#define Scale 8
	#elif(normalized_scale==5)
		#define Scale 11
	#else
		#define Scale 0
	#endif	

	#define  NT		(Scale*Scale)
	#define  NC		(2*Scale*Scale-2*Scale)
	
#elif(TaskGraphType==FFT)
	#define  Scale  normalized_scale
	#define  NT		( (Scale+2)*(1<<Scale) - 1 )
	#define  NC		(  ( 2*(Scale+1)*(1<<Scale) ) - 2  )
	
#elif(TaskGraphType==ForkJoin)

	#if(normalized_scale==1)
		#define Scale 1
	#elif(normalized_scale==2)
		#define Scale 3
	#elif(normalized_scale==3)
		#define Scale 6
	#elif(normalized_scale==4)
		#define Scale 12
	#elif(normalized_scale==5)
		#define Scale 25
	#else
		#define Scale 0
	#endif	

	#define  NT		( 5*Scale+1 )
	#define  NC		( 8*Scale   )
	
#elif(TaskGraphType==GaussianElimination)

	#if(normalized_scale==1)
		#define Scale 2
	#elif(normalized_scale==2)
		#define Scale 4
	#elif(normalized_scale==3)
		#define Scale 6
	#elif(normalized_scale==4)
		#define Scale 9
	#elif(normalized_scale==5)
		#define Scale 14
	#else
		#define Scale 0
	#endif	

	#define  NT		( (Scale*Scale+5*Scale)>>1 )
	#define  NC		(  Scale*Scale+4*Scale-3)
	
#else
	#define  NT		0
	#define  NC		0
	
#endif


// ����Ķ� 
#if(TEST_MODE==1)
	#define  CT_ELEMENT_MAXIMAL 1 // ����ģʽ�������趨������������
	#define  CC_ELEMENT_MAXIMAL 1 // ����ģʽ�������趨������������
#elif(CCR_Grade==-2)
	#define  CT_ELEMENT_MAXIMAL 540
	#define  CC_ELEMENT_MAXIMAL 60
#elif(CCR_Grade==-1)
	#define  CT_ELEMENT_MAXIMAL 540
	#define  CC_ELEMENT_MAXIMAL 180
#elif(CCR_Grade==0)
	#define  CT_ELEMENT_MAXIMAL 180
	#define  CC_ELEMENT_MAXIMAL 180
#elif(CCR_Grade==1)
	#define  CT_ELEMENT_MAXIMAL 180
	#define  CC_ELEMENT_MAXIMAL 540
#elif(CCR_Grade==2)
	#define  CT_ELEMENT_MAXIMAL 60
	#define  CC_ELEMENT_MAXIMAL 540
#else
	#define  CT_ELEMENT_MAXIMAL 0
	#define  CC_ELEMENT_MAXIMAL 0
#endif

// ����Ķ� 
#define TIME_MAX_LIMIT 65535

// ����Ķ� 
#define DEFAULT_TASK_PROCESSOR_SELECT 0
#define DEFAULT_COMMUNICATION_START_TIME 0
#define	DEFAULT_TASK_START_TIME 0

///////////////////////////////////////////////////////////////////////////////////////
/// SLS

int 
SLS // �򻯾�̬�б�����㷨
(
	// CT: cost of task ����ִ��ʱ��
	// CC: cost of communication ͨ������ʱ��
	int CT[NT],  
	int CC[NT][NT],
	
	int schedule_list[NT], // �������ȼ��б�
	
	int task_processor_select[NT], // ÿ���������ڵĴ���Ԫ
	int communication_start_time[NT][NT], // ����֮���ͨ����ʼʱ��
	int task_start_time[NT] // ����֮���ִ����ʼʱ��
);


void 
single_task_schedule_for_sls
(
	int CT[NT], 
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task
);
	
	
int
get_best_processor_for_current_task_for_sls
(
	int CT[NT], 
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task
);


int 
get_current_task_eft_on_processor_for_sls // ��� ��ǰ������ָ������Ԫ�ϵ� �������ʱ��
(
	int CT[NT],
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task,
	int processor
);
		
		
void
schedule_current_task_on_best_processor_for_sls
(
	int CT[NT], 
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task,
	int best_processor_for_current_task
);

		
///////////////////////////////////////////////////////////////////////////////////////		
// ETF

 // ѡ��������翪ʼʱ�������ִ��,����ѡ���������ȼ��ϸߵ�����ִ��
int // level 0
ETF
(
	int CT[NT],  
	int CC[NT][NT],
	int bl_list[NT],
	int schedule_list[NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
);

int // level 1
select_current_task_for_etf
(
	int CT[NT],  
	int CC[NT][NT],
	int bl_list[NT],
	int schedule_list[NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
);

int // level 2
judge_task_free_for_etf
(
	int CC[NT][NT],
	int task_processor_select[NT],
	int task_index
);
		
int // level 2
get_current_task_est_for_etf
(
	int CT[NT],
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task
);


int // level 3
get_current_task_est_on_processor_for_etf // ��� ��ǰ������ָ������Ԫ�ϵ� �������ʱ��
(
	int CT[NT],
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task,
	int processor
);
///////////////////////////////////////////////////////////////////////////////////////	
// OC

void
copy_schedule_result
(
	int task_processor_select_temp[NT],
	int communication_start_time_temp[NT][NT],
	int task_start_time_temp[NT],
	
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
);

void copy_one_dimension_array ( int array_input[NT], int array_output[NT] );

int  task_order_judge(int schedule_list[NT],int CC[NT][NT]);

void next_array(int list[NT], int array_length );

void swap(int* x, int* y);

int  factorial(int x);

int OC
(	
	int CT[NT],
	int CC[NT][NT],
	int schedule_list[NT],
	int NBT, // ÿ����������������Ŀ. ��Ҫע��: NTδ������NBT, ������һ�������Ҫ��������.
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
);

///////////////////////////////////////////////////////////////////////////////////////
// rsc

void
swap_array_element_for_rsc // ��������������Ӧ������Ԫ��
(
int array_i[NT],  // ��������
int index_a, 
int index_b, 
int array_o[NT]  // �������
);

int
rsc	// random-swap-choose-task-schedule-algorithm
(
int CT[NT],
int CC[NT][NT],
int schedule_list[NT],
int SLS_schedule_times_limit, // ָ�� ETF���ȴ��� �� ����
int task_processor_select[NT],
int communication_start_time[NT][NT],
int task_start_time[NT]
);

///////////////////////////////////////////////////////////////////////////////////////

void
prt_one_dimension_array
(
	int array[NT],
	int array_length	
);

///////////////////////////////////////////////////////////////////////////////////////
// ����ͼ�ĳ�ʼ��

void
initialize_task_graph
(
	int sample_index,
	int CT[NT],
	int CC[NT][NT]
);

void set_value_by_level_for_GaussianElimination
(
	int cc_temp[NC],
	int CC[NT][NT]
);

///////////////////////////////////////////////////////////////////////////////////////
// ����Ȩ�����ȼ��б�

int  focus_judge(int CC[NT][NT], int task_index); 
void get_bl_list ( int CT[NT], int CC[NT][NT], int BL_LIST[NT] );
int  get_bl_value( int CT[NT], int CC[NT][NT], int task_index );

int  source_judge(int CC[NT][NT], int task_index);
void get_tl_list ( int CT[NT], int CC[NT][NT], int TL_LIST[NT] );
int  get_tl_value( int CT[NT], int CC[NT][NT], int task_index );

void get_schedule_list_with_bl_and_tl
(
	int BL_LIST[NT],
	int TL_LIST[NT],
	int schedule_list[NT]
);

void get_schedule_list_with_bl
(
	int BL_LIST[NT],
	int schedule_list[NT]
);

void get_schedule_list_with_tl
(
	int TL_LIST[NT],
	int schedule_list[NT]
);

///////////////////////////////////////////////////////////////////////////////////////
// ͳ����Ϣ
void
get_statistics_data
(
	int sls_sl[SAMPLE_NUMBER],
	int etf_sl[SAMPLE_NUMBER],
	int oc_sl[SAMPLE_NUMBER],
	int rsc_sl[SAMPLE_NUMBER],
	int compare_result[3][6],
	float accelerate_compare_result[3][6]
);

///////////////////////////////////////////////////////////////////////////////////////
void
display_sche_res
(
	int schedule_length,
	int schedule_list_for_sls[NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
)
{

	printf("\n-----Schedule Result Start-----\n \n");
	
	printf("schedule_length=%d.\n",schedule_length);
	
	printf("Task_Processor_Select:\n");
	prt_one_dimension_array(task_processor_select,NT);
	printf("\n");
	
	printf("Task_Start_Time:\n");
	prt_one_dimension_array(task_start_time,NT);
	printf("\n-----Schedule Result End-----\n \n");
}				

int main(void)
{

	int etf_sl[SAMPLE_NUMBER];
	int rsc_sl[SAMPLE_NUMBER][4];
	
	int sample_index;
	for(sample_index=0;sample_index<SAMPLE_NUMBER;sample_index++) // ͳ��(SAMPLE_NUMBER>1)���߲���(SAMPLE_NUMBER=1)
	{
	
		int CT[NT];
		int CC[NT][NT];
		initialize_task_graph(sample_index,CT,CC);
		
		
		
		int tl_list[NT];
		get_tl_list(CT,CC,tl_list);
		
		int bl_list[NT];
		get_bl_list(CT,CC,bl_list);
		
		
		
		int schedule_list_with_tl[NT];
		get_schedule_list_with_tl(tl_list,schedule_list_with_tl);
		
		int schedule_list_with_bl_1_temp[NT];
		get_schedule_list_with_bl(bl_list,schedule_list_with_bl_1_temp);
		
		int schedule_list_with_bl_2_temp[NT];
		get_schedule_list_with_bl(bl_list,schedule_list_with_bl_2_temp);
		
		int schedule_list_with_bl_and_tl[NT];
		get_schedule_list_with_bl_and_tl(bl_list,schedule_list_with_bl_and_tl);
		
		int schedule_list_with_random[NT];
		int i;
		for(i=0;i<NT;i++)
		{
			schedule_list_with_random[i]=i;
		}
		
		
		int task_processor_select[NT];
		int communication_start_time[NT][NT];
		int task_start_time[NT];
		
		
		int etf_schedule_length=ETF
									(
										CT,
										CC,
										bl_list,
										schedule_list_with_bl_1_temp,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
		
		
		
		//////////////////////////////////// rsc 
		
		int rsc_sls_schedule_times_limit=1024; // ����OC�㷨��RSC�㷨����ETF���ȵĴ����������
		
		int rsc_schedule_length_0=	rsc // �򻯾�̬�б�����㷨
									(
										CT,
										CC,
										schedule_list_with_bl_2_temp,
										rsc_sls_schedule_times_limit,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
									
		int rsc_schedule_length_1=	rsc // �򻯾�̬�б�����㷨
									(
										CT,
										CC,
										schedule_list_with_tl,
										rsc_sls_schedule_times_limit,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
									
		int rsc_schedule_length_2=	rsc // �򻯾�̬�б�����㷨
									(
										CT,
										CC,
										schedule_list_with_bl_and_tl,
										rsc_sls_schedule_times_limit,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
		
		int rsc_schedule_length_3=	rsc // �򻯾�̬�б�����㷨
									(
										CT,
										CC,
										schedule_list_with_random,
										rsc_sls_schedule_times_limit,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
									
		int rsc_schedule_length=TIME_MAX_LIMIT;
		
		if(rsc_schedule_length_0<rsc_schedule_length)
			rsc_schedule_length=rsc_schedule_length_0;
		
		if(rsc_schedule_length_1<rsc_schedule_length)
			rsc_schedule_length=rsc_schedule_length_1;
		
		if(rsc_schedule_length_2<rsc_schedule_length)
			rsc_schedule_length=rsc_schedule_length_2;
			
		if(rsc_schedule_length_3<rsc_schedule_length)
			rsc_schedule_length=rsc_schedule_length_3;
			
		
		etf_sl[sample_index]=etf_schedule_length;
		rsc_sl[sample_index][0]=rsc_schedule_length_0;
		rsc_sl[sample_index][1]=rsc_schedule_length_1;
		rsc_sl[sample_index][2]=rsc_schedule_length_2;
		rsc_sl[sample_index][3]=rsc_schedule_length_3;
		
		if( ( (sample_index+1)%10) == 0 )
		{
			printf("%d samples completed.\n",sample_index+1);
			printf("\n");
		}
		else
		{
			;
		}
		
	}
	
	FILE *fp=fopen("txt.txt","w");
	if(fp==NULL)
	{
		printf("Error. Line=%d.\n",__LINE__);
	}
	else
	{
		int j;
		for(j=0;j<SAMPLE_NUMBER;j++)
		{
			fprintf(fp,"%6d\n",etf_sl[sample_index]);
			fprintf
			(
				fp,"%6d,%6d,%6d,%6d.\n",
				rsc_sl[sample_index][0],
				rsc_sl[sample_index][1],
				rsc_sl[sample_index][2],
				rsc_sl[sample_index][3]
			);
			fprintf(fp,"\n \n");
		}
	}
	
	flcose(fp);

}

///////////////////////////////////////////////////////////////////////////////////////////////
// SLS

// �򻯾�̬�б�����㷨
int 
SLS 
(
	int CT[NT],  
	int CC[NT][NT],  
	
	int schedule_list[NT],
	
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
)
{

	// ��ʼ�����Ƚ��
	int i,j;
	for(i=0;i<NT;i++)
	{
		task_processor_select[i]=DEFAULT_TASK_PROCESSOR_SELECT;
		task_start_time[i]=DEFAULT_TASK_START_TIME;
		for(j=0;j<NT;j++)
		{
			communication_start_time[i][j]=DEFAULT_COMMUNICATION_START_TIME;
		}
	}
	
	// ����ȡ��һ��������е���
	int k;
	for(k=0;k<NT;k++)
	{
		int current_task=schedule_list[k];
		single_task_schedule_for_sls
		(
			CT,
			CC,
			task_processor_select,
			communication_start_time,
			task_start_time,
			current_task
		);
	}
	
	// ��� ������������ʱ������ֵ max_task_finish_time ��Ϊ ���ȳ��� Schedule_Length
	int max_task_finish_time=0;
	int v;
	for(v=0;v<NT;v++)
	{
		if(task_start_time[v]+CT[v]> max_task_finish_time)
		{
			max_task_finish_time=task_start_time[v]+CT[v];
		}
		else
		{
			;
		}
	}
	
	return(max_task_finish_time);

}


// ������������ŵ���
void 
single_task_schedule_for_sls
(
	int CT[NT], 
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task
)
{
	int best_processor_for_current_task=get_best_processor_for_current_task_for_sls
										(
											CT,
											CC,
											task_processor_select,
											communication_start_time,
											task_start_time,
											current_task
										);
	
	schedule_current_task_on_best_processor_for_sls
	(
		CT,
		CC,
		task_processor_select,
		communication_start_time,
		task_start_time,
		current_task,
		best_processor_for_current_task
	);
}


//Ϊ��ǰ����ѡ����Ѵ���Ԫ
int
get_best_processor_for_current_task_for_sls
(
	int CT[NT], 
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task
)
{
	int best_processor_for_current_task=0;  // ��Ҫע��: ����Ԫ��Ŵ�1��NP, �����Ŵ�0��(NT-1)
	
	int processor;
	int min_current_task_etf=TIME_MAX_LIMIT;
	for(processor=1;processor<NP+1;processor++)
	{
	// eft: earliest finish time
		int current_task_eft_on_processor=get_current_task_eft_on_processor_for_sls // ��� ��ǰ������ָ������Ԫ�ϵ� �������ʱ��
										  (
												CT,
												CC,
												task_processor_select,
												communication_start_time,
												task_start_time,
												current_task,
												processor
										  );
										  
		if(current_task_eft_on_processor<min_current_task_etf)
		{
			min_current_task_etf=current_task_eft_on_processor;
			best_processor_for_current_task=processor;
		}
		else
		{
			;
		}
		
	}
	
	return(best_processor_for_current_task);
	
}


// ��� ��ǰ������ָ������Ԫ�ϵ� �������ʱ��
int 
get_current_task_eft_on_processor_for_sls
(
	int CT[NT],
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task,
	int processor
)
{
	
	int i;
	int max_task_finish_time_on_processor=0;
	for(i=0;i<NT;i++)
	{
		if
		(
			(task_processor_select[i]==processor)
			&&
			( (task_start_time[i]+CT[i]) > max_task_finish_time_on_processor )
		)
		{
			max_task_finish_time_on_processor=task_start_time[i]+CT[i];
		}
		else
		{
			;
		}
	}
	
	int max_pred_communication_finish_time=0;
	for(i=0;i<NT;i++)
	{
		if
		(
			(CC[i][current_task]<TIME_MAX_LIMIT) // ǰ����ϵ
			&&
			(task_processor_select[i]!=processor)
		)
		{
			int pred_communication_finish_time=task_start_time[i]+CT[i]+CC[i][current_task];
			if(pred_communication_finish_time>max_pred_communication_finish_time)
			{
				max_pred_communication_finish_time=pred_communication_finish_time;
			}
			else
			{
				;
			}
		}
	}
	
	int task_earliest_start_time_on_processor=
			( max_task_finish_time_on_processor > max_pred_communication_finish_time)?
			  max_task_finish_time_on_processor : max_pred_communication_finish_time ;
	
	int task_earliest_finish_time_on_processor= task_earliest_start_time_on_processor+CT[current_task];

	return(task_earliest_finish_time_on_processor);
	
}

// �ѵ�ǰ�����������Ѵ���Ԫ��
void
schedule_current_task_on_best_processor_for_sls
(
	int CT[NT], 
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task,
	int best_processor_for_current_task
)
{
	int current_task_eft_on_best_processor=	get_current_task_eft_on_processor_for_sls // ��� ��ǰ������ָ������Ԫ�ϵ� �������ʱ��
											(
												CT,
												CC,
												task_processor_select,
												communication_start_time,
												task_start_time,
												current_task,
												best_processor_for_current_task
											);

	task_start_time[current_task]= current_task_eft_on_best_processor-CT[current_task];

	
	// ���� communication_start_time[NT][NT]
	int i;
	for(i=0;i<NT;i++) // ����ǰ���ڵ�
	{
		if
		(
			(CC[i][current_task]<TIME_MAX_LIMIT)
			&&
			(task_processor_select[i]!=best_processor_for_current_task)
		)
		{
			communication_start_time[i][current_task]=task_start_time[i]+CT[i];
		}
	}
	
	task_processor_select[current_task]=best_processor_for_current_task;
	
}

/////////////////////////////////////////////////////////////////////////////////////

int
ETF 
(
	int CT[NT],  
	int CC[NT][NT],
	int bl_list[NT],
	int schedule_list[NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
)
{
	
	// ��ʼ�����Ƚ��
	int i,j;
	for(i=0;i<NT;i++)
	{
		task_processor_select[i]=DEFAULT_TASK_PROCESSOR_SELECT;
		task_start_time[i]=DEFAULT_TASK_START_TIME;
		for(j=0;j<NT;j++)
		{
			communication_start_time[i][j]=DEFAULT_COMMUNICATION_START_TIME;
		}
	}
	
	// printf("Enetr ETF Function.\n");
	
	
	// ����ȡ��һ��������е���
	int k;
	for(k=0;k<NT;k++)
	{
		
		int current_task=	select_current_task_for_etf
							(
								CT,
								CC,
								bl_list,
								schedule_list,
								task_processor_select,
								communication_start_time,
								task_start_time
							);
							
		//  printf("k=%d, current_task=%d.\n",k,current_task);
		
		single_task_schedule_for_sls
		(
			CT,
			CC,
			task_processor_select,
			communication_start_time,
			task_start_time,
			current_task
		);
	}
	
	// ��� ������������ʱ������ֵ max_task_finish_time ��Ϊ ���ȳ��� Schedule_Length
	int max_task_finish_time=0;
	int v;
	for(v=0;v<NT;v++)
	{
		if(task_start_time[v]+CT[v]> max_task_finish_time)
		{
			max_task_finish_time=task_start_time[v]+CT[v];
		}
		else
		{
			;
		}
	}
	
	return(max_task_finish_time);

}

int
select_current_task_for_etf
(
	int CT[NT],  
	int CC[NT][NT],
	int bl_list[NT],
	int schedule_list[NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
)
{

	// printf("Enter Function select_current_task_for_etf.\n");

	int free[NT]; // ÿ�������Ƿ�����
	int j;
	for(j=0;j<NT;j++)
	{
		free[j]=judge_task_free_for_etf(CC,task_processor_select,j);
	}
	
	// printf("Free:\n"); prt_one_dimension_array(free,NT);
	
	
	int current_task;
	int min_est=TIME_MAX_LIMIT;
	int i;
	for(i=0;i<NT;i++)
	{
		if(free[i]==1) // ������������
		{
			// printf("task i=%d.\n",i);
			int est=	get_current_task_est_for_etf
						(
							CT,
							CC,
							task_processor_select,
							communication_start_time,
							task_start_time,
							i
						);
						
			// printf("task i=%d, est=%d.\n",i,est);
			
			if(est<min_est)
			{
				// printf("Enter If.\n");
				min_est=est;
				current_task=i;
			}
			else if(est==min_est)
			{
				// printf("Enter Else If.\n");
				if( bl_list[i] > bl_list[current_task] )
				{
					current_task=i;
				}
				else
				{
					;
				}
			}
			else
			{
				// printf("Enter Else.\n");
				;
			}
		}
	}
	return(current_task);
}

// �жϽڵ��Ƿ�����
int
judge_task_free_for_etf
(
	int CC[NT][NT],
	int task_processor_select[NT],
	int task_index
) 
{

	// �����ѱ����ȣ�������ɣ�������ֵ
	if(task_processor_select[task_index]!=DEFAULT_TASK_PROCESSOR_SELECT)
	{
		return(0);
	}
	
	// ����ǰ��δ�����ȣ�������ɣ�������ֵ
	int i;
	for(i=0;i<NT;i++) 
	{
		if( (CC[i][task_index]<TIME_MAX_LIMIT) && (task_processor_select[i]==DEFAULT_TASK_PROCESSOR_SELECT) )
		{
			return(0);
		}
	}
	
	return(1);
}


// ѡȡ��ǰ����
int
get_current_task_est_for_etf
(
	int CT[NT],
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task
)
{
	int processor;
	int min_est=TIME_MAX_LIMIT;
	for(processor=1;processor<NP+1;processor++)
	{
		int est=get_current_task_est_on_processor_for_etf
				(
					CT,
					CC,
					task_processor_select,
					communication_start_time,
					task_start_time,
					current_task,
					processor
				);
		if(est<min_est)
		{
			min_est=est;
		}
	}
	return(min_est);
}

// ��� ��ǰ������ָ������Ԫ�ϵ� �������ʱ��
int 
get_current_task_est_on_processor_for_etf
(
	int CT[NT],
	int CC[NT][NT],
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT],
	int current_task,
	int processor
)
{
	
	int i;
	int max_task_finish_time_on_processor=0;
	for(i=0;i<NT;i++)
	{
		if
		(
			(task_processor_select[i]==processor)
			&&
			( (task_start_time[i]+CT[i]) > max_task_finish_time_on_processor )
		)
		{
			max_task_finish_time_on_processor=task_start_time[i]+CT[i];
		}
	}
	
	int max_pred_communication_finish_time=0;
	for(i=0;i<NT;i++)
	{
		if
		(
			(CC[i][current_task]<TIME_MAX_LIMIT)
			&&
			(task_processor_select[i]!=processor)
		)
		{
			int pred_communication_finish_time=task_start_time[i]+CT[i]+CC[i][current_task];
			if(pred_communication_finish_time>max_pred_communication_finish_time)
			{
				max_pred_communication_finish_time=pred_communication_finish_time;
			}
		}
	}
	
	int task_earliest_start_time_on_processor=
			( max_task_finish_time_on_processor > max_pred_communication_finish_time)?
			  max_task_finish_time_on_processor : max_pred_communication_finish_time ;

	return(task_earliest_start_time_on_processor);
	
}



/////////////////////////////////////////////////////////////////////////////////////
// OC

void
copy_schedule_result
(
	int task_processor_select_temp[NT],
	int communication_start_time_temp[NT][NT],
	int task_start_time_temp[NT],
	
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
)
{
	int i;
	for(i=0;i<NT;i++)
	{
		task_processor_select[i]=task_processor_select_temp[i];
		int k;
		for(k=0;k<NT;k++)
		{
			communication_start_time[i][k]=communication_start_time_temp[i][k];
		}
		task_start_time[i]=task_start_time_temp[i];
	}
}


void copy_one_dimension_array ( int array_input[NT], int array_output[NT] )
{
	int i;
	for(i=0;i<NT;i++)
	{
		array_output[i]=array_input[i];
	}
}

int task_order_judge(int schedule_list[NT],int CC[NT][NT])
{
	int right_task_order=1;
	int i;
	for(i=0;i<NT;i++)
	{
		int j;
		for(j=0;j<NT;j++)
		{
			int real_task_i=schedule_list[i];
			int real_task_j=schedule_list[j];
			if( (i<j)&&(CC[real_task_j][real_task_i]<TIME_MAX_LIMIT) )
			{
				right_task_order=0;
			}
		}
	}
	return(right_task_order);
}


// ����ʵ�ֿ������ȼ��������
void  next_array(int list[], int array_length )
{
	int exit_loop=0; 
	int replaced_pos=0; 
	int i;
	for(i=array_length-1;i>0;i--)
	{  
		if(list[i]>list[i-1])
		{  
			exit_loop=1;
			replaced_pos=i-1;  
			break;
		} 
	}   
	 
	if(exit_loop==0)
	{   
		;
	}
	
	else
	{  
		for(i=array_length-1;i>0;i--)
		{
			if(list[i]>list[replaced_pos])
			{  
				swap(&list[i],&list[replaced_pos]);
				break;
			} 
		}

		for(i=replaced_pos+1;i<=(int)((replaced_pos+array_length-1)/2);i++)
		{ 
			swap(&list[i],&list[array_length+replaced_pos-i]);
		} 

	}
		
}

void swap(int* x, int* y)
{
	int x1=*x;
	int y1=*y;
	*x=y1;
	*y=x1;
}	

int factorial(int x)
{
	int result=1;
	if(x==1)
	{
		result=1;
	}
	else
	{
		int i;
		for(i=1;i<=x;i++)
		{
			result=result*i;
		}
	}	
	return(result);
}


int OC
(
	int CT[NT],
	int CC[NT][NT],
	int schedule_list[NT],
	int NBT, // ÿ����������������Ŀ. ��Ҫע��: NTδ������NBT, ������һ�������Ҫ��������.
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
)
{
	int sls_schedule_times=0; // OC�㷨��SLS���ȴ���. sls_schedule_times��Ϊ����ֵ������communication_start_time[0][0]
	
	int minimum_program_finish_time_inside_block_search=TIME_MAX_LIMIT; // ����ֵ�ĳ�ʼ��
	
	int NB=(int)(NT/NBT);
	int last_block_task_number=NT-NB*NBT; // ���һ������������
	
	int block_index;
	for(block_index=0;block_index<NB;block_index++) // ���ε���ÿ�����
	{
		
		int real_schedule_list_temp[NT];
		
		int block_inside_better_schedule_list_number=0;
		int tpo_index; 
		for(tpo_index=0;tpo_index<factorial(NBT);tpo_index++)
		{

			// list �����������˳��
			int list[NT]; // list �ں�NTԪ��, ��Ч��ֻ��ǰNBTԪ��
			if(tpo_index==0)
			{
				int i;
				for(i=0;i<NT;i++) { list[i]=i; } // ��ʼ�� list
			}
			else
			{
				next_array(list,NBT);
			}
			
			int real_schedule_list[NT];
			copy_one_dimension_array(schedule_list, real_schedule_list);
			int i; for(i=0;i<NBT;i++) { real_schedule_list[block_index*NBT+i] = schedule_list[ block_index*NBT+list[i] ]; }
			
			int task_processor_select_temp[NT];
			int communication_start_time_temp[NT][NT];
			int task_start_time_temp[NT];
			int right_task_order=task_order_judge(real_schedule_list,CC);
			if(right_task_order==1)
			{
				int program_finish_time_inside_block_search=SLS
															(
																CT,CC,
																real_schedule_list,
																task_processor_select_temp,
																communication_start_time_temp,
																task_start_time_temp
															);
				sls_schedule_times= sls_schedule_times + 1;
				//
				/*	printf("Schedule_List_Temp:\n");prt_one_dimension_array(real_schedule_list,NT); printf("\n");
					printf("program_finish_time=%d.\n",program_finish_time_inside_block_search); // */
				if( program_finish_time_inside_block_search < minimum_program_finish_time_inside_block_search )
				{

					minimum_program_finish_time_inside_block_search=program_finish_time_inside_block_search ;
					copy_one_dimension_array(real_schedule_list,real_schedule_list_temp);
					copy_schedule_result
					(
						task_processor_select_temp,	communication_start_time_temp,	task_start_time_temp,
						task_processor_select,		communication_start_time,		task_start_time
					);
					
					block_inside_better_schedule_list_number=block_inside_better_schedule_list_number+1;
				}
			}
		}
		
		if(block_inside_better_schedule_list_number>0)
		{
			copy_one_dimension_array(real_schedule_list_temp,schedule_list);
		}
	}

	if(last_block_task_number==0) // û�в�����
	{
		;
	}
	else // ���в�����
	{
		int real_schedule_list_temp[NT];
		int block_inside_better_schedule_list_number=0;
		int minimum_program_finish_time_inside_block_search=TIME_MAX_LIMIT;
		int tpo_index; 
		for(tpo_index=0;tpo_index<factorial(last_block_task_number);tpo_index++)
		{
			// list �����������˳��
			int list[NT]; // list �ں�NTԪ��, ��Ч��ֻ��ǰlast_block_task_numberԪ��
			if(tpo_index==0)
			{
				int i;
				for(i=0;i<NT;i++) { list[i]=i; } // ��ʼ�� list
			}
			else
			{
				next_array(list,last_block_task_number);
			}

			int real_schedule_list[NT];
			copy_one_dimension_array(schedule_list, real_schedule_list);
			int i;
			for(i=0;i<last_block_task_number;i++) { real_schedule_list[NB*NBT+i] = schedule_list[ NB*NBT+list[i] ]; }

			int task_processor_select_temp[NT];
			int communication_start_time_temp[NT][NT];
			int task_start_time_temp[NT];
			int right_task_order=task_order_judge(real_schedule_list,CC);
			if(right_task_order==1)
			{
				int program_finish_time_inside_block_search=SLS
															(
																CT,CC,
																real_schedule_list,
																task_processor_select_temp,
																communication_start_time_temp,
																task_start_time_temp
															);
				sls_schedule_times= sls_schedule_times + 1;
				// 
				/*	printf("Schedule_List_Temp:\n");prt_one_dimension_array(real_schedule_list,NT); printf("\n");
					printf("program_finish_time=%d.\n",program_finish_time_inside_block_search); // */
				if( program_finish_time_inside_block_search < minimum_program_finish_time_inside_block_search )
				{

					minimum_program_finish_time_inside_block_search=program_finish_time_inside_block_search ;
					copy_one_dimension_array(real_schedule_list,real_schedule_list_temp);
					copy_schedule_result
					(
						task_processor_select_temp,	communication_start_time_temp,	task_start_time_temp,
						task_processor_select,		communication_start_time,		task_start_time
					);
					block_inside_better_schedule_list_number=block_inside_better_schedule_list_number+1;
				}
			}
		}

		if(block_inside_better_schedule_list_number>0)
		{
			copy_one_dimension_array(real_schedule_list_temp,schedule_list);
		}
		
	}
	
	// OC�㷨��SLS���ȴ���. sls_schedule_times��Ϊ����ֵ������communication_start_time[0][0]
	communication_start_time[0][0]=sls_schedule_times;
	
	return(minimum_program_finish_time_inside_block_search);
	
}


/////////////////////////////////////////////////////////////////////////////////////
// RSC

void
swap_array_element_for_rsc // ��������������Ӧ������Ԫ��
(
int array_i[NT],  // ��������
int index_a, 
int index_b, 
int array_o[NT]  // �������
)
{
	copy_one_dimension_array(array_i,array_o);

	array_o[index_a]=array_i[index_b];
	array_o[index_b]=array_i[index_a];

}


int
rsc // random-swap-choose-task-schedule-algorithm
(
int CT[NT],
int CC[NT][NT],
int schedule_list[NT],
int SLS_schedule_times_limit, // ָ�� ETF���ȴ��� �� ����
int task_processor_select[NT],
int communication_start_time[NT][NT],
int task_start_time[NT]
)
{

	int swap_times=0;   						// �Ի��Ĵ���
	int swap_times_from_last_update=0;			// �����ϴθ��µĶԻ�����
	int SLS_schedule_times=0;					// SLS���ȵĴ���
	int SLS_schedule_times_from_last_update=0;	//�����ϴθ��µ�SLS���ȴ���
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////****************************Caution******************************************/////////////////
	// ע�⣺ �˴����� int min_finish_time=TIME_MAX_LIMIT; // ��Ȼ����
	int min_finish_time=SLS
						(
							CT, CC,
							schedule_list,
							task_processor_select,
							communication_start_time,
							task_start_time
						);
						
	while(SLS_schedule_times<SLS_schedule_times_limit)
	{

		int element_1_index= ( rand() ) % (NT);// int element_1_index= ( rand() ) % NT; // NT����Ҫ������,�����ܲ��������! ���ǹŹ�!
		int element_2_index= ( rand() ) % (NT);// int element_2_index= ( rand() ) % NT; // NT����Ҫ������,�����ܲ��������! ���ǹŹ�!
		// printf("SWAP-ELEMNET:%d,%d\n\n",element_1_index,element_2_index);
		
		int schedule_list_temp[NT];
		swap_array_element_for_rsc
		(
			schedule_list,
			element_1_index,
			element_2_index,
			schedule_list_temp
		);
		
		swap_times=swap_times+1; // �Ի�����
		swap_times_from_last_update=swap_times_from_last_update+1;
		
		// �ж� schedule_list_temp_temp �Ƿ�����������ȼ�����
		if( task_order_judge(schedule_list_temp,CC)==1 )
		{
			SLS_schedule_times=SLS_schedule_times+1;
			
			int task_processor_select_temp[NT];
			int communication_start_time_temp[NT][NT];
			int task_start_time_temp[NT];
			int finish_time=ETF
							(
								CT, CC,
								schedule_list_temp,
								task_processor_select_temp,
								communication_start_time_temp,
								task_start_time_temp
							);
						
			if(finish_time< min_finish_time )
			{
				SLS_schedule_times_from_last_update=0;
				swap_times_from_last_update=0;
				
				min_finish_time= finish_time;
				copy_one_dimension_array(schedule_list_temp,schedule_list);
				copy_schedule_result
				(
					task_processor_select_temp,
					communication_start_time_temp,
					task_start_time_temp,
					task_processor_select,
					communication_start_time,
					task_start_time
				);
			}
			else
			{
				SLS_schedule_times_from_last_update=SLS_schedule_times_from_last_update+1;
			}

		}

	}
	
	return(min_finish_time);
	
}


/////////////////////////////////////////////////////////////////////////////////////


void
initialize_task_graph
(
	int sample_index,
	int CT[NT],
	int CC[NT][NT]
)
{
	
	int cc_temp[NC];

	if(TEST_MODE==1)
	{
		CT[0]=10;
		CT[1]=16;
		CT[2]=18;
		CT[3]=13;
		CT[4]=9;
		CT[5]=9;
		CT[6]=10;
		CT[7]=15;
		CT[8]=3;
		CT[9]=3;
		CT[10]=18;
		CT[11]=2;
	
		cc_temp[0]=12;
		cc_temp[1]=40;
		cc_temp[2]=33;
		cc_temp[3]=15;
		cc_temp[4]=18;
		cc_temp[5]=57;
		cc_temp[6]=3;
		cc_temp[7]=13;
		cc_temp[8]=27;
		cc_temp[9]=40;
		cc_temp[10]=55;
		cc_temp[11]=42;
		cc_temp[12]=4;
		cc_temp[13]=45;
		
		int i;
		for(i=0;i<NT;i++) 
		{
			int j;
			for(j=0;j<NT;j++)
			{
				CC[i][j]=TIME_MAX_LIMIT;
			}
		}
	
		CC[0][1]=cc_temp[0];
		CC[0][2]=cc_temp[1];
		CC[0][3]=cc_temp[2];
			
		CC[1][8]=cc_temp[3];
		CC[1][9]=cc_temp[4];
		CC[1][10]=cc_temp[5];
			
		CC[2][7]=cc_temp[6];
		CC[2][11]=cc_temp[7];
			
		CC[3][4]=cc_temp[8];
		CC[3][5]=cc_temp[9];
		CC[3][6]=cc_temp[10];
		CC[3][7]=cc_temp[11];
			
		CC[9][11]=cc_temp[12];
		CC[10][11]=cc_temp[13];	
	}
	else
	{
		srand(sample_index);
		int v;
		for(v=0;v<NT;v++)
		{
			CT[v]= ( rand() ) % (CT_ELEMENT_MAXIMAL) + 1 ;
		}
		
		srand(SAMPLE_NUMBER-1-sample_index);
		int s;
		for(s=0;s<NC;s++)
		{
			cc_temp[s]= ( rand() ) % (CC_ELEMENT_MAXIMAL) + 1;
		}
		
		int u;
		for(u=0;u<NT;u++) 
		{
			int t;
			for(t=0;t<NT;t++)
			{
				CC[u][t]=TIME_MAX_LIMIT;
			}
		}
		
		if(TaskGraphType==Input)
		{
			int i;
			for(i=0;i<NC;i++)
			{
				CC[i][(NC/2)+1+(int)(i/2)]=cc_temp[i];
			}
		}
		
		else if(TaskGraphType==Output)
		{
			int i;
			for(i=0;i<NC;i++)
			{
				CC[(int)(i/2)][i+1]=cc_temp[i];
			}
		}		

		else if(TaskGraphType==MeanValue)
		{
			int  cnt_1_temp=0;
			int i;
			for(i=0;i<NT-1;i++)
			{
				int j;
				for(j=i+1;j<NT;j++)
				{
					if(  (int)(i/4)==(int)(j/4) && ( 1 + i-(int)(i/4)*4 ) == ( j-(int)(j/4)*4 )  )
					{
						CC[i][j]=cc_temp[cnt_1_temp];
						cnt_1_temp=cnt_1_temp+1;
					}
					if(  1 + (int)(i/4)==(int)(j/4) && ( i-(int)(i/4)*4 ) == ( j-(int)(j/4)*4 )  )
					{
						CC[i][j]=cc_temp[cnt_1_temp];
						cnt_1_temp=cnt_1_temp+1;
					}
				}
			}
		}

		else if(TaskGraphType==FFT)
		{
			// ���帳ֵ
			int cnt_temp=0;
			if(NT==5)
			{
				CC[0][1]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[0][2]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[1][4]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[2][3]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
			}
			else if(NT==15)
			{
				CC[0][1]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;

				CC[0][2]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;

				CC[1][3]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[1][4]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;

				CC[2][5]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[2][6]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[3][7]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[3][8]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[4][7]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[4][8]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[5][9]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[5][10]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[6][9]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[6][10]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[7][11]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[7][13]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[8][12]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[8][14]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[9][11]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[9][13]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				
				CC[10][12]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[10][14]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
			}
			
			else if(NT==39)
			{
				int i;
				for(i=0;i<=6;i++)
				{
					CC[i][2*i+1]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
				
					CC[i][2*i+2]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
				}
				
				CC[7][15]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[7][16]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[8][15]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[8][16]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[9][17]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[9][18]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[10][17]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[10][18]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[11][19]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[11][20]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[12][19]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[12][20]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[13][21]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[13][22]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[14][21]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[14][22]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[15][23]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[15][25]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[16][24]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[16][26]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[17][23]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[17][25]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[18][24]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[18][26]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[19][27]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[19][29]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[20][28]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[20][30]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[21][27]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[21][29]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[22][28]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[22][30]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[23][31]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[23][35]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[24][32]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[24][36]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[25][33]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[25][37]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[26][34]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[26][38]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[27][31]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[27][35]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[28][32]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[28][36]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[29][33]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[29][37]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[30][34]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
				CC[30][38]=cc_temp[cnt_temp];
				cnt_temp=cnt_temp+1;
				
			}	
			
			else if(NT==95)
			{
				int i;
				for(i=0;i<=14;i++)
				{
					CC[i][2*i+1]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[i][2*i+2]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
				}
				
				for(i=0;i<8;i++)
				{
					CC[2*i+15][2*i+31]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[2*i+15][2*i+32]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[2*i+16][2*i+31]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[2*i+16][2*i+32]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
				}
				
				for(i=0;i<4;i++)
				{
					CC[4*i+31][4*i+47]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[4*i+31][4*i+49]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[4*i+32][4*i+48]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[4*i+32][4*i+50]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[4*i+33][4*i+47]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[4*i+33][4*i+49]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[4*i+34][4*i+48]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[4*i+34][4*i+50]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
				}
				
				for(i=0;i<2;i++)
				{
					CC[8*i+47][8*i+63]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+47][8*i+67]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+48][8*i+64]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+48][8*i+68]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+49][8*i+65]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+49][8*i+69]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+50][8*i+66]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+50][8*i+70]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+51][8*i+63]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+51][8*i+67]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+52][8*i+64]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+52][8*i+68]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+53][8*i+65]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+53][8*i+69]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+54][8*i+66]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
					CC[8*i+54][8*i+70]=cc_temp[cnt_temp];
					cnt_temp=cnt_temp+1;
					
				}
				
				{
					for(i=63;i<=78;i++)
					{
						CC[i][i+16]==cc_temp[cnt_temp];
						cnt_temp=cnt_temp+1;
					}
					
					for(i=63;i<=70;i++)
					{
						CC[i][i+24]==cc_temp[cnt_temp];
						cnt_temp=cnt_temp+1;
					}
					
					for(i=71;i<=78;i++)
					{
						CC[i][i+8]==cc_temp[cnt_temp];
						cnt_temp=cnt_temp+1;
					}
					
				}
			}
			
		}
		
		else if(TaskGraphType==ForkJoin)
		{
			int i;
			for(i=0;i<NC;i++)
			{
				int sk=(int)(i/4);
				int tk=sk%2;
				int uk=(int)(i/8);
				if(tk==0)
				{
					CC[5*uk][5*uk+1]=cc_temp[8*uk+0];
					CC[5*uk][5*uk+2]=cc_temp[8*uk+1];
					CC[5*uk][5*uk+3]=cc_temp[8*uk+2];
					CC[5*uk][5*uk+4]=cc_temp[8*uk+3];
				}
				else
				{
					CC[5*uk+1][5*uk+5]=cc_temp[8*uk+4];
					CC[5*uk+2][5*uk+5]=cc_temp[8*uk+5];
					CC[5*uk+3][5*uk+5]=cc_temp[8*uk+6];
					CC[5*uk+4][5*uk+5]=cc_temp[8*uk+7];
				}
			}
		}
		else if(TaskGraphType==GaussianElimination)
		{
			set_value_by_level_for_GaussianElimination(cc_temp,CC);
		}
		else
		{
			printf("ERROR:%d.\n",__LINE__);
		}
		
		
	}
}

void set_value_by_level_for_GaussianElimination
(
	int cc_temp[NC],
	int CC[NT][NT]
)
{
	int cnt_4_temp=0;
	int level;
	int start_node_at_top=0;
	for(level=0;level<Scale;level++) // level ���������ʾ���� Scale ��һ�����ʾ����
	{
		int number_nodes_at_bottom=Scale+1-level;
		int i;
		for(i=0;i<number_nodes_at_bottom;i++) // ����
		{
			CC[start_node_at_top][start_node_at_top+1+i]=cc_temp[cnt_4_temp];
			cnt_4_temp=cnt_4_temp+1;
		}
		
		if( level!= (Scale-1) )
		{
			CC[start_node_at_top][start_node_at_top+number_nodes_at_bottom+1]=cc_temp[cnt_4_temp];
			cnt_4_temp=cnt_4_temp+1;
			
			CC[start_node_at_top+2][start_node_at_top+number_nodes_at_bottom+1]=cc_temp[cnt_4_temp];
			cnt_4_temp=cnt_4_temp+1;
			
			for(i=0;i<number_nodes_at_bottom-1;i++)
			{
				CC[start_node_at_top+2+i][start_node_at_top+2+i+number_nodes_at_bottom]=cc_temp[cnt_4_temp];
				cnt_4_temp=cnt_4_temp+1;
			}
		}
		else
		{
			;
		}
		start_node_at_top=start_node_at_top+Scale+2-level;
	}	
}


/////////////////////////////////////////////////////////////////////////////////////

void
prt_one_dimension_array
(
	int array[],
	int array_length	
)
{
	int i;
	for(i=0;i<array_length;i++)
	{
		printf("%d ,",array[i]);
	}
}

/////////////////////////////////////////////////////////////////////////////////////




int focus_judge(int CC[NT][NT], int task_index)
{
	int i;
	for(i=0;i<NT;i++)
	{
		if(CC[task_index][i]<TIME_MAX_LIMIT) // ���к��,������ǻ��
			return(0);
	}
	return(1);
	
}

void get_bl_list ( int CT[NT], int CC[NT][NT], int BL_LIST[NT] )
{
	int i;
	for( i=0;i<NT;i++)
	{
		BL_LIST[i]=get_bl_value(CT,CC,i);
	}
}
int  get_bl_value( int CT[NT], int CC[NT][NT], int task_index )
{
	if(focus_judge(CC,task_index)==1) // ���
	{
		return( CT[task_index] );
	}
	else
	{	
		int bl_value_max=0;
		int j;
		for(j=0;j<NT;j++)
		{
			if(CC[task_index][j]<TIME_MAX_LIMIT)
			{
				int bl_value_temp = get_bl_value(CT,CC,j)+CC[task_index][j]+CT[task_index];
				if(bl_value_temp>bl_value_max)
				{
					bl_value_max=bl_value_temp;
				}	
			}
		}
		return(bl_value_max);
	}
}

int source_judge(int CC[NT][NT], int task_index)
{
	int i;
	for(i=0;i<NT;i++)
	{
		if(CC[i][task_index]<TIME_MAX_LIMIT) // ����ǰ��,�������Դ��
			return(0);
	}
	return(1);
}

void get_tl_list ( int CT[NT], int CC[NT][NT], int TL_LIST[NT] )
{
	int i;
	for( i=0;i<NT;i++)
	{
		TL_LIST[i]=get_tl_value(CT,CC,i);
	}
}

int  get_tl_value( int CT[NT], int CC[NT][NT], int task_index )
{
	if(source_judge(CC,task_index)==1) //Դ��
	{
		return(0);
	}
	else
	{
		int tl_value_max=0;
		int j;
		for( j=0;j<NT;j++)
		{
			if(CC[j][task_index]<TIME_MAX_LIMIT)
			{
				int tl_value_temp = get_tl_value(CT,CC,j)+CT[j]+CC[j][task_index];
				if(tl_value_temp>tl_value_max)	
				{
					tl_value_max=tl_value_temp;
				}
			}
		}
		return(tl_value_max);
	}
}

void get_schedule_list_with_bl_and_tl
(
	int BL_LIST[NT],
	int TL_LIST[NT],
	int schedule_list[NT]
)
{
	int i;
	for(i=0;i<NT;i++)
	{
		int bigger=0; // ���ڵ�ǰԪ��ֵ��Ԫ���ж��ٸ�
		int j;
		for(j=0;j<NT;j++)
		{
			if(BL_LIST[j]>BL_LIST[i])
			{
				bigger=bigger+1;
			}
			else if( (BL_LIST[j]==BL_LIST[i]) && (TL_LIST[j]>TL_LIST[i]) )
			{
				bigger=bigger+1;
			}
			else if( (BL_LIST[j]==BL_LIST[i]) && (TL_LIST[j]==TL_LIST[i]) && (j>i) )
			{
				bigger=bigger+1;
			}
			else
			{
				;
			}
		}
		schedule_list[bigger]=i;
	}
}

void get_schedule_list_with_bl
(
	int BL_LIST[NT],
	int schedule_list[NT]
)
{
	int i,j;
	for(i=0;i<NT;i++)
	{
		int bigger=0; // ���ڵ�ǰԪ��ֵ��Ԫ���ж��ٸ�
		for(j=0;j<NT;j++)
		{
			if(BL_LIST[j]>BL_LIST[i])
			{
				bigger=bigger+1;
			}
			else if( (BL_LIST[j]==BL_LIST[i]) && (j>i) )
			{
				bigger=bigger+1;
			}
			else
			{
				;
			}
		}
		schedule_list[bigger]=i; 
		// bl_list[i]��ֵԽС, i��schedule_list�е�λ��bigger��ֵԽ��
	}
}

void get_schedule_list_with_tl
(
	int TL_LIST[NT],
	int schedule_list[NT]
)
{
	int i,j;
	for(i=0;i<NT;i++)
	{
		int bigger=0; // ���ڵ�ǰԪ��ֵ��Ԫ���ж��ٸ�
		for(j=0;j<NT;j++)
		{
			if(TL_LIST[j]>TL_LIST[i])
			{
				bigger=bigger+1;
			}
			else if( (TL_LIST[j]==TL_LIST[i]) && (j>i) )
			{
				bigger=bigger+1;
			}
			else
			{
				;
			}
		}
		schedule_list[bigger]=i; 
		// tl_list[i]��ֵԽС, i��schedule_list�е�λ��bigger��ֵԽ��
	}
}

void
get_statistics_data
(
	int sls_sl[SAMPLE_NUMBER],
	int etf_sl[SAMPLE_NUMBER],
	int oc_sl[SAMPLE_NUMBER],
	int rsc_sl[SAMPLE_NUMBER],
	int better_or_bader_compare_result[3][6],
	float accelerate_compare_result[3][6]
)
{
	int x;
	for(x=0;x<3;x++)
	{
		int y;
		for(y=0;y<6;y++)
		{
			better_or_bader_compare_result[x][y]=0;
			accelerate_compare_result[x][y]=0.0;
		}
	}

	int i;
	for(i=0;i<SAMPLE_NUMBER;i++)
	{
		
		if(etf_sl[i]<sls_sl[i])
		{
			better_or_bader_compare_result[0][0]= better_or_bader_compare_result[0][0]+1;
		}
		else if(etf_sl[i]==sls_sl[i])
		{
			better_or_bader_compare_result[1][0]= better_or_bader_compare_result[1][0]+1;
		}
		else
		{
			better_or_bader_compare_result[2][0]= better_or_bader_compare_result[2][0]+1;
		}
		
		if(oc_sl[i]<sls_sl[i])
		{
			better_or_bader_compare_result[0][1]= better_or_bader_compare_result[0][1]+1;
		}
		else if(oc_sl[i]==sls_sl[i])
		{
			better_or_bader_compare_result[1][1]= better_or_bader_compare_result[1][1]+1;
		}
		else
		{
			better_or_bader_compare_result[2][1]= better_or_bader_compare_result[2][1]+1;
		}
		
		if(oc_sl[i]<etf_sl[i])
		{
			better_or_bader_compare_result[0][2]= better_or_bader_compare_result[0][2]+1;
		}
		else if(oc_sl[i]==etf_sl[i])
		{
			better_or_bader_compare_result[1][2]= better_or_bader_compare_result[1][2]+1;
		}
		else
		{
			better_or_bader_compare_result[2][2]= better_or_bader_compare_result[2][2]+1;
		}		

		if(rsc_sl[i]<sls_sl[i])
		{
			better_or_bader_compare_result[0][3]= better_or_bader_compare_result[0][3]+1;
		}
		else if(oc_sl[i]==sls_sl[i])
		{
			better_or_bader_compare_result[1][3]= better_or_bader_compare_result[1][3]+1;
		}
		else
		{
			better_or_bader_compare_result[2][3]= better_or_bader_compare_result[2][3]+1;
		}
	
		if(rsc_sl[i]<etf_sl[i])
		{
			better_or_bader_compare_result[0][4]= better_or_bader_compare_result[0][4]+1;
		}
		else if(rsc_sl[i]==etf_sl[i])
		{
			better_or_bader_compare_result[1][4]= better_or_bader_compare_result[1][4]+1;
		}
		else
		{
			better_or_bader_compare_result[2][4]= better_or_bader_compare_result[2][4]+1;
		}	
		
		if(rsc_sl[i]<oc_sl[i])
		{
			better_or_bader_compare_result[0][5]= better_or_bader_compare_result[0][5]+1;
		}
		else if(rsc_sl[i]==etf_sl[i])
		{
			better_or_bader_compare_result[1][5]= better_or_bader_compare_result[1][5]+1;
		}
		else
		{
			better_or_bader_compare_result[2][5]= better_or_bader_compare_result[2][5]+1;
		}
		
	}
	
	
	float max_acc_etf_sls=0;
	float avg_acc_etf_sls=0;
	float min_acc_etf_sls=TIME_MAX_LIMIT;
	
	float max_acc_oc_sls=0;
	float avg_acc_oc_sls=0;
	float min_acc_oc_sls=TIME_MAX_LIMIT;
	
	float max_acc_oc_etf=0;
	float avg_acc_oc_etf=0;
	float min_acc_oc_etf=TIME_MAX_LIMIT;
	
	float max_acc_rsc_sls=0;
	float avg_acc_rsc_sls=0;
	float min_acc_rsc_sls=TIME_MAX_LIMIT;
	
	float max_acc_rsc_etf=0;
	float avg_acc_rsc_etf=0;
	float min_acc_rsc_etf=TIME_MAX_LIMIT;
	
	float max_acc_rsc_oc=0;
	float avg_acc_rsc_oc=0;
	float min_acc_rsc_oc=TIME_MAX_LIMIT;
	
	int j;
	for(j=0;j<SAMPLE_NUMBER;j++)
	{
		float acc;
		
		acc=( (float)sls_sl[j] )   /   ( (float)etf_sl[j] ) ;
		max_acc_etf_sls=(acc>max_acc_etf_sls)?acc:max_acc_etf_sls;
		avg_acc_etf_sls=avg_acc_etf_sls+ ( (float)acc )/ ( (float)SAMPLE_NUMBER );
		min_acc_etf_sls=(acc<min_acc_etf_sls)?acc:min_acc_etf_sls;
		
		acc=( (float)sls_sl[j] )   /   ( (float)oc_sl[j] ) ;
		max_acc_oc_sls=(acc>max_acc_oc_sls)?acc:max_acc_oc_sls;
		avg_acc_oc_sls=avg_acc_oc_sls+ ( (float)acc )/ ( (float)SAMPLE_NUMBER );
		min_acc_oc_sls=(acc<min_acc_oc_sls)?acc:min_acc_oc_sls;
		
		acc=( (float)etf_sl[j] )   /   ( (float)oc_sl[j] ) ;
		max_acc_oc_etf=(acc>max_acc_oc_etf)?acc:max_acc_oc_etf;
		avg_acc_oc_etf=avg_acc_oc_etf+ ( (float)acc )/ ( (float)SAMPLE_NUMBER );
		min_acc_oc_etf=(acc<min_acc_oc_etf)?acc:min_acc_oc_etf;
		
		acc=( (float)sls_sl[j] )   /   ( (float)rsc_sl[j] ) ;
		max_acc_rsc_sls=(acc>max_acc_rsc_sls)?acc:max_acc_rsc_sls;
		avg_acc_rsc_sls=avg_acc_rsc_sls+ ( (float)acc )/ ( (float)SAMPLE_NUMBER );
		min_acc_rsc_sls=(acc<min_acc_rsc_sls)?acc:min_acc_rsc_sls;
		
		acc=( (float)etf_sl[j] )   /   ( (float)rsc_sl[j] ) ;
		max_acc_rsc_etf=(acc>max_acc_rsc_etf)?acc:max_acc_rsc_etf;
		avg_acc_rsc_etf=avg_acc_rsc_etf+ ( (float)acc )/ ( (float)SAMPLE_NUMBER );
		min_acc_rsc_etf=(acc<min_acc_rsc_etf)?acc:min_acc_rsc_etf;
		
		acc=( (float)oc_sl[j] )   /   ( (float)rsc_sl[j] ) ;
		max_acc_rsc_oc=(acc>max_acc_rsc_oc)?acc:max_acc_rsc_oc;
		avg_acc_rsc_oc=avg_acc_rsc_oc+ ( (float)acc )/ ( (float)SAMPLE_NUMBER );
		min_acc_rsc_oc=(acc<min_acc_rsc_oc)?acc:min_acc_rsc_oc;
	}
	
	accelerate_compare_result[0][0]=max_acc_etf_sls;
	accelerate_compare_result[1][0]=avg_acc_etf_sls;
	accelerate_compare_result[2][0]=min_acc_etf_sls;
	
	accelerate_compare_result[0][1]=max_acc_oc_sls;
	accelerate_compare_result[1][1]=avg_acc_oc_sls;
	accelerate_compare_result[2][1]=min_acc_oc_sls;
	
	accelerate_compare_result[0][2]=max_acc_oc_etf;
	accelerate_compare_result[1][2]=avg_acc_oc_etf;
	accelerate_compare_result[2][2]=min_acc_oc_etf;
	
	accelerate_compare_result[0][3]=max_acc_rsc_sls;
	accelerate_compare_result[1][3]=avg_acc_rsc_sls;
	accelerate_compare_result[2][3]=min_acc_rsc_sls;
	
	accelerate_compare_result[0][4]=max_acc_rsc_etf;
	accelerate_compare_result[1][4]=avg_acc_rsc_etf;
	accelerate_compare_result[2][4]=min_acc_rsc_etf;
	
	accelerate_compare_result[0][5]=max_acc_rsc_oc;
	accelerate_compare_result[1][5]=avg_acc_rsc_oc;
	accelerate_compare_result[2][5]=min_acc_rsc_oc;
}


