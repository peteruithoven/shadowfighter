/*
 *  Blob.cpp
 *  ShadowFighter
 *
 *  Created by Peter Uithoven on 4/22/10.
 */

#include "Blob.h"


Blob::Blob()
{
	color = 0xffffff;
}

int Blob::operator<(const Blob * blob) const
{
	return ( this->boundingRect.x < blob->boundingRect.x);
}
int Blob::operator>(const Blob * blob) const
{
	return ( this->boundingRect.x > blob->boundingRect.x);
}
int Blob::operator<(const Blob blob) const
{
	return ( this->boundingRect.x < blob.boundingRect.x);
}
int Blob::operator>(const Blob blob) const
{
	return ( this->boundingRect.x > blob.boundingRect.x);
}

int Blob::operator<(Blob * blob)
{
	return ( this->boundingRect.x < blob->boundingRect.x);
}
int Blob::operator>(Blob * blob)
{
	return ( this->boundingRect.x > blob->boundingRect.x);
}
int Blob::operator<(Blob blob)
{
	return ( this->boundingRect.x < blob.boundingRect.x);
}
int Blob::operator>(Blob blob)
{
	return ( this->boundingRect.x > blob.boundingRect.x);
}