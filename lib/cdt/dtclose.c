#include	<cdt/dthdr.h>
#include	<stddef.h>

/*	Close a dictionary
**
**	Written by Kiem-Phong Vo (05/25/96)
*/
int dtclose(Dt_t* dt)
{
	Dtdisc_t	*disc;
	int		ev = 0;

	if(!dt || dt->nview > 0 ) /* can't close if being viewed */
		return -1;

	disc = dt->disc;

	if(dt->view)	/* turn off viewing */
		dtview(dt,NULL);

	if(ev == 0) /* release all allocated data */
	{	(void)dt->meth->searchf(dt, NULL, DT_CLEAR);
		if(dtsize(dt) > 0)
			return -1;

		if(dt->data->ntab > 0)
			dt->memoryf(dt, dt->data->htab, 0, disc);
		dt->memoryf(dt, dt->data, 0, disc);
	}

	if(dt->type == DT_MALLOC)
		free(dt);
	else if(ev == 0 && dt->type == DT_MEMORYF)
		dt->memoryf(dt, dt, 0, disc);

	return 0;
}
