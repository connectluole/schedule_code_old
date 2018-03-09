// 先看 SLS(Simplified List Scheduling) 算法
// SLS 算法 : 首先计算各个任务的优先权值 BL(BL/TL), 接着对所有任务的优先权值进行优先级排序,最后再根据优先级高低依次调度各个任务.
// 如何调度单个任务: 任务被调度在能够最早开始执行的处理单元上.
// 

// int debug=0;

////////////////////////////////////////////////////////////////////////////////////////////////
// 参数配置开始

#define  TEST_MODE				0  		// 选择测试模式还是统计模式

#define  TaskGraphType 			2		// 可改 取值 1,2,3,4,5,6 // 测试模式下无所谓取值
#define  normalized_scale		3 		// 可改 决定任务数目, 取值 1,2,3,4,5

#define	 NP						4		// 可改 // 处理单元编号是从1到NP // 任务编号是从0到(NT-1)
#define  CCR_Grade				1		// 可改 决定通信计算比值 // -2:1/9, -1:1/3, 0:1, +1:1*3, +2:1*9 // CCR决定边值与点值的上限
#define  MAX_BLOCK_TASK_NUMBER_FOR_OC  6// 可改 这与OC算法相关


// 参数配置结束
////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// 六种任务图类型,无需改动 
#define  Input					1
#define  Output					2
#define  MeanValue				3
#define  FFT					4
#define  ForkJoin				5
#define  GaussianElimination	6

///////////////////////////////////

#if(TEST_MODE==1)
	#define  SAMPLE_NUMBER		1    // 无需改动
#else
	#define  SAMPLE_NUMBER		500  // 可改 样本数目
#endif

///////////////////////////////////
// 无需改动 
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


// 无需改动 
#if(TEST_MODE==1)
	#define  CT_ELEMENT_MAXIMAL 1 // 测试模式下随意设定正的整数即可
	#define  CC_ELEMENT_MAXIMAL 1 // 测试模式下随意设定正的整数即可
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

// 无需改动 
#define TIME_MAX_LIMIT 65535

// 无需改动 
#define DEFAULT_TASK_PROCESSOR_SELECT 0
#define DEFAULT_COMMUNICATION_START_TIME 0
#define	DEFAULT_TASK_START_TIME 0

///////////////////////////////////////////////////////////////////////////////////////
/// SLS

int 
SLS // 简化静态列表调度算法
(
	// CT: cost of task 任务执行时间
	// CC: cost of communication 通信消耗时间
	int CT[NT],  
	int CC[NT][NT],
	
	int schedule_list[NT], // 任务优先级列表
	
	int task_processor_select[NT], // 每个任务所在的处理单元
	int communication_start_time[NT][NT], // 任务之间的通信起始时间
	int task_start_time[NT] // 任务之间的执行起始时间
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
get_current_task_eft_on_processor_for_sls // 获得 当前任务在指定处理单元上的 最早完成时间
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

 // 选择具有最早开始时间的任务执行,否则选择任务优先级较高的任务执行
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
get_current_task_est_on_processor_for_etf // 获得 当前任务在指定处理单元上的 最早完成时间
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
	int NBT, // 每个宏块所含任务的数目. 需要注意: NT未必整除NBT, 因此最后一个宏块需要单独考虑.
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
);

///////////////////////////////////////////////////////////////////////////////////////
// rsc

void
swap_array_element_for_rsc // 交换两个索引对应的数组元素
(
int array_i[NT],  // 输入数组
int index_a, 
int index_b, 
int array_o[NT]  // 输出数组
);

int
rsc	// random-swap-choose-task-schedule-algorithm
(
int CT[NT],
int CC[NT][NT],
int schedule_list[NT],
int SLS_schedule_times_limit, // 指定 ETF调度次数 的 上限
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
// 任务图的初始化

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
// 优先权与优先级列表

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
// 统计信息
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
	for(sample_index=0;sample_index<SAMPLE_NUMBER;sample_index++) // 统计(SAMPLE_NUMBER>1)或者测试(SAMPLE_NUMBER=1)
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
		
		int rsc_sls_schedule_times_limit=1024; // 保持OC算法与RSC算法进行ETF调度的次数必须相等
		
		int rsc_schedule_length_0=	rsc // 简化静态列表调度算法
									(
										CT,
										CC,
										schedule_list_with_bl_2_temp,
										rsc_sls_schedule_times_limit,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
									
		int rsc_schedule_length_1=	rsc // 简化静态列表调度算法
									(
										CT,
										CC,
										schedule_list_with_tl,
										rsc_sls_schedule_times_limit,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
									
		int rsc_schedule_length_2=	rsc // 简化静态列表调度算法
									(
										CT,
										CC,
										schedule_list_with_bl_and_tl,
										rsc_sls_schedule_times_limit,
										task_processor_select,
										communication_start_time,
										task_start_time
									);
		
		int rsc_schedule_length_3=	rsc // 简化静态列表调度算法
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

// 简化静态列表调度算法
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

	// 初始化调度结果
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
	
	// 依次取出一个任务进行调度
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
	
	// 求出 所有任务的完成时间的最大值 max_task_finish_time 作为 调度长度 Schedule_Length
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


// 单个任务的最优调度
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


//为当前任务选择最佳处理单元
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
	int best_processor_for_current_task=0;  // 需要注意: 处理单元编号从1到NP, 任务编号从0到(NT-1)
	
	int processor;
	int min_current_task_etf=TIME_MAX_LIMIT;
	for(processor=1;processor<NP+1;processor++)
	{
	// eft: earliest finish time
		int current_task_eft_on_processor=get_current_task_eft_on_processor_for_sls // 获得 当前任务在指定处理单元上的 最早完成时间
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


// 获得 当前任务在指定处理单元上的 最早完成时间
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
			(CC[i][current_task]<TIME_MAX_LIMIT) // 前驱关系
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

// 把当前任务调度在最佳处理单元上
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
	int current_task_eft_on_best_processor=	get_current_task_eft_on_processor_for_sls // 获得 当前任务在指定处理单元上的 最早完成时间
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

	
	// 更新 communication_start_time[NT][NT]
	int i;
	for(i=0;i<NT;i++) // 搜索前驱节点
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
	
	// 初始化调度结果
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
	
	
	// 依次取出一个任务进行调度
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
	
	// 求出 所有任务的完成时间的最大值 max_task_finish_time 作为 调度长度 Schedule_Length
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

	int free[NT]; // 每个任务是否自由
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
		if(free[i]==1) // 搜索自由任务
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

// 判断节点是否自由
int
judge_task_free_for_etf
(
	int CC[NT][NT],
	int task_processor_select[NT],
	int task_index
) 
{

	// 任务已被调度，则非自由，返回零值
	if(task_processor_select[task_index]!=DEFAULT_TASK_PROCESSOR_SELECT)
	{
		return(0);
	}
	
	// 存在前驱未被调度，则非自由，返回零值
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


// 选取当前任务
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

// 获得 当前任务在指定处理单元上的 最早完成时间
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


// 用于实现块内优先级排序遍历
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
	int NBT, // 每个宏块所含任务的数目. 需要注意: NT未必整除NBT, 因此最后一个宏块需要单独考虑.
	int task_processor_select[NT],
	int communication_start_time[NT][NT],
	int task_start_time[NT]
)
{
	int sls_schedule_times=0; // OC算法的SLS调度次数. sls_schedule_times作为返回值隐藏在communication_start_time[0][0]
	
	int minimum_program_finish_time_inside_block_search=TIME_MAX_LIMIT; // 返回值的初始化
	
	int NB=(int)(NT/NBT);
	int last_block_task_number=NT-NB*NBT; // 最后一个宏块的任务数
	
	int block_index;
	for(block_index=0;block_index<NB;block_index++) // 依次调度每个宏块
	{
		
		int real_schedule_list_temp[NT];
		
		int block_inside_better_schedule_list_number=0;
		int tpo_index; 
		for(tpo_index=0;tpo_index<factorial(NBT);tpo_index++)
		{

			// list 代表任务调度顺序
			int list[NT]; // list 内含NT元素, 有效地只有前NBT元素
			if(tpo_index==0)
			{
				int i;
				for(i=0;i<NT;i++) { list[i]=i; } // 初始化 list
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

	if(last_block_task_number==0) // 没有残余宏块
	{
		;
	}
	else // 还有残余宏块
	{
		int real_schedule_list_temp[NT];
		int block_inside_better_schedule_list_number=0;
		int minimum_program_finish_time_inside_block_search=TIME_MAX_LIMIT;
		int tpo_index; 
		for(tpo_index=0;tpo_index<factorial(last_block_task_number);tpo_index++)
		{
			// list 代表任务调度顺序
			int list[NT]; // list 内含NT元素, 有效地只有前last_block_task_number元素
			if(tpo_index==0)
			{
				int i;
				for(i=0;i<NT;i++) { list[i]=i; } // 初始化 list
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
	
	// OC算法的SLS调度次数. sls_schedule_times作为返回值隐藏在communication_start_time[0][0]
	communication_start_time[0][0]=sls_schedule_times;
	
	return(minimum_program_finish_time_inside_block_search);
	
}


/////////////////////////////////////////////////////////////////////////////////////
// RSC

void
swap_array_element_for_rsc // 交换两个索引对应的数组元素
(
int array_i[NT],  // 输入数组
int index_a, 
int index_b, 
int array_o[NT]  // 输出数组
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
int SLS_schedule_times_limit, // 指定 ETF调度次数 的 上限
int task_processor_select[NT],
int communication_start_time[NT][NT],
int task_start_time[NT]
)
{

	int swap_times=0;   						// 对换的次数
	int swap_times_from_last_update=0;			// 距离上次更新的对换次数
	int SLS_schedule_times=0;					// SLS调度的次数
	int SLS_schedule_times_from_last_update=0;	//距离上次更新的SLS调度次数
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////****************************Caution******************************************/////////////////
	// 注意： 此处不能 int min_finish_time=TIME_MAX_LIMIT; // 不然出错
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

		int element_1_index= ( rand() ) % (NT);// int element_1_index= ( rand() ) % NT; // NT必须要带括号,否则不能产生随机数! 真是古怪!
		int element_2_index= ( rand() ) % (NT);// int element_2_index= ( rand() ) % NT; // NT必须要带括号,否则不能产生随机数! 真是古怪!
		// printf("SWAP-ELEMNET:%d,%d\n\n",element_1_index,element_2_index);
		
		int schedule_list_temp[NT];
		swap_array_element_for_rsc
		(
			schedule_list,
			element_1_index,
			element_2_index,
			schedule_list_temp
		);
		
		swap_times=swap_times+1; // 对换次数
		swap_times_from_last_update=swap_times_from_last_update+1;
		
		// 判断 schedule_list_temp_temp 是否符合任务优先级排序
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
			// 具体赋值
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
	for(level=0;level<Scale;level++) // level 从零数起表示层数 Scale 从一数起表示几层
	{
		int number_nodes_at_bottom=Scale+1-level;
		int i;
		for(i=0;i<number_nodes_at_bottom;i++) // 层内
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
		if(CC[task_index][i]<TIME_MAX_LIMIT) // 具有后继,因而不是汇点
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
	if(focus_judge(CC,task_index)==1) // 汇点
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
		if(CC[i][task_index]<TIME_MAX_LIMIT) // 具有前驱,因而不是源点
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
	if(source_judge(CC,task_index)==1) //源点
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
		int bigger=0; // 大于当前元素值的元素有多少个
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
		int bigger=0; // 大于当前元素值的元素有多少个
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
		// bl_list[i]数值越小, i在schedule_list中的位置bigger数值越大
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
		int bigger=0; // 大于当前元素值的元素有多少个
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
		// tl_list[i]数值越小, i在schedule_list中的位置bigger数值越大
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


