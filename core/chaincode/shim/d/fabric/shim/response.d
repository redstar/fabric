module fabric.shim.response;

import protos;

Response success(string payload = "")
{
    return Response();
}

Response success(byte[] payload)
{
    return Response();
}
