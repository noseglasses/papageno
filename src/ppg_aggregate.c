

static void ppg_aggregate_reset(PPG_Aggregate *aggregate) 
{
	ppg_token_reset((PPG_Token__*)aggregate);
	
	aggregate->n_inputs_active = 0;
	
	for(uint8_t i = 0; i < aggregate->n_members; ++i) {
		aggregate->member_active[i] = false;
	}
}

static void ppg_aggregate_deallocate_member_storage(PPG_Aggregate *aggregate) {	
	
	if(aggregate->inputs) {
		free(aggregate->inputs);
		aggregate->inputs = NULL;
	}
	if(aggregate->member_active) {
		free(aggregate->member_active);
		aggregate->member_active = NULL;
	}
}

static void ppg_aggregate_resize(PPG_Aggregate *aggregate, 
							uint8_t n_members)
{
	ppg_aggregate_deallocate_member_storage(aggregate);
	
	aggregate->n_members = n_members;
	aggregate->inputs = (PPG_Input *)malloc(n_members*sizeof(PPG_Input));
	aggregate->member_active = (bool *)malloc(n_members*sizeof(bool));
	aggregate->n_inputs_active = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		aggregate->member_active[i] = false;
		ppg_init_input(&aggregate->inputs[i]);
	}
}

static PPG_Aggregate* ppg_aggregate_destroy(PPG_Aggregate *aggregate) {
	
	ppg_aggregate_deallocate_member_storage(aggregate);

	return aggregate;
}

static PPG_Aggregate *ppg_aggregate_alloc(void){
    return (PPG_Aggregate*)malloc(sizeof(PPG_Aggregate));
}

static bool ppg_aggregates_equal(PPG_Aggregate *c1, PPG_Aggregate *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	uint8_t n_equalities = 0;
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		for(uint8_t j = 0; j < c1->n_members; ++j) {
			if(ppg_context->input_id_equal(c1->inputs[i].input_id, c2->inputs[j].input_id)) { 
				++n_equalities;
				break;
			}
		}
	}
	
	return n_equalities == c1->n_members;
}

static void *ppg_aggregate_new(void *aggregate) {
    
	/* Explict call to parent constructor
	 */
	ppg_token_new((PPG_Token__*)aggregate);
	
	PPG_Chord *chord = (PPG_Chord*)aggregate;
		
	/* Initialize the aggregate
	 */
	chord->n_members = 0;
	chord->inputs = NULL;
	chord->member_active = NULL;
	chord->n_inputs_active = 0;

	return aggregate;
}

static PPG_Token ppg_initialize_aggregate(	
								PPG_Aggregate *aggregate,
								uint8_t n_inputs,
								PPG_Input inputs[])
{
	ppg_aggregate_resize(aggregate, n_inputs);
	 
	for(uint8_t i = 0; i < n_inputs; ++i) {
		aggregate->inputs[i] = inputs[i];
	}
	 
	/* Return the new end of the pattern 
	 */
	return aggregate;
}