#include "anchor_db.h"

typedef struct {
	bool			is_valid;
	uint16_t		anchor_id;
} anchor_db_entry_t;

static anchor_db_entry_t	m_anchors[ANCHOR_DB_SIZE];
static int					m_last_index;

void anchor_db_init()
{
	for(int i = 0; i < ANCHOR_DB_SIZE; i++)
	{
		m_anchors[i].is_valid = false;
	}

	m_last_index = 0;
}

bool anchor_db_add(uint16_t anchor_id)
{
	if(m_last_index >= ANCHOR_DB_SIZE)
		return false;

	for(int i = 0; i < m_last_index; i++)
	{
		if(m_anchors[i].anchor_id == anchor_id &&
				m_anchors[i].is_valid)
			return false;
	}

	m_anchors[m_last_index].is_valid = true;
	m_anchors[m_last_index].anchor_id = anchor_id;

	m_last_index++;

	return true;
}

int anchor_db_get_size() {
	return m_last_index;
}

uint16_t anchor_db_get_anchor(int index) {
	return m_anchors[index].anchor_id;
}
