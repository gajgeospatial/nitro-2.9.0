#include <import/nrt.h>
#include <import/j2k.h>

NRT_BOOL writeFile(j2k_Container *container, nrt_Uint32 x0, nrt_Uint32 y0,
                   nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 *buf,
                   nrt_Uint64 bufSize, nrt_Error *error)
{
    NRT_BOOL rc = NRT_SUCCESS;
    char filename[NRT_MAX_PATH];
    nrt_IOHandle outHandle;

    NRT_SNPRINTF(filename, NRT_MAX_PATH, "raw-%d_%d__%d_%d.out", x0, y0, x1, y1);
    outHandle = nrt_IOHandle_create(filename, NRT_ACCESS_WRITEONLY, NRT_CREATE,
                                    error);
    if (NRT_INVALID_HANDLE(outHandle))
    {
        goto CATCH_ERROR;
    }
    if (!nrt_IOHandle_write(outHandle, (const char *) buf, bufSize, error))
    {
        goto CATCH_ERROR;
    }
    printf("Wrote file: %s\n", filename);

    goto CLEANUP;

    CATCH_ERROR:
    {
        rc = NRT_FAILURE;
    }
    CLEANUP:
    {
        if (!NRT_INVALID_HANDLE(outHandle))
            nrt_IOHandle_close(outHandle);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    int argIt = 0;
    char *fname = NULL;
    nrt_Error error;
    j2k_Container *container = NULL;
    nrt_Uint64 bufSize;
    nrt_Uint32 x0 = 0;
    nrt_Uint32 y0 = 0;
    nrt_Uint32 x1 = 0;
    nrt_Uint32 y1 = 0;
    nrt_Uint8 *buf = NULL;

    for (argIt = 1; argIt < argc; ++argIt)
    {
        if (strcmp(argv[argIt], "--x0") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            x0 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--y0") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            y0 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--x1") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            x1 = atoi(argv[++argIt]);
        }
        else if (strcmp(argv[argIt], "--y1") == 0)
        {
            if (argIt >= argc - 1)
                goto CATCH_ERROR;
            y1 = atoi(argv[++argIt]);
        }
        else if (!fname)
        {
            fname = argv[argIt];
        }
    }

    if (!fname)
    {
        printf("Usage: %s [--x0 --y0 --x1 --y1] <j2k-file>\n", argv[0]);
        goto CATCH_ERROR;
    }

    if (!(container = j2k_Container_open(fname, &error)))
        goto CATCH_ERROR;

    if (x1 == 0)
        x1 = j2k_Container_getWidth(container, &error);
    if (y1 == 0)
        y1 = j2k_Container_getHeight(container, &error);

    if ((bufSize = j2k_Container_readRegion(container, x0, y0, x1, y1, &buf,
                                            &error)) == 0)
    {
        goto CATCH_ERROR;
    }

    if (!writeFile(container, x0, y0, x1, y1, buf, bufSize, &error))
    {
        goto CATCH_ERROR;
    }

    goto CLEANUP;

    CATCH_ERROR:
    {
        nrt_Error_print(&error, stdout, "Exiting...");
        rc = 1;
    }
    CLEANUP:
    {
        if (container)
            j2k_Container_destruct(&container);
        if (buf)
            NRT_FREE(buf);
    }
    return rc;

}