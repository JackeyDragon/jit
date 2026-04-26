
array[float] get_img_nr(array[float] all_imgs, int nr){
    int idx = nr * 784;
    float img[5] = {0,0,0,0,0};
    int i = 0;
    lbl_get_img_nr:
    if (i == 5)
goto ret_get_img_nr;
    img[i] = all_imgs[idx+i];
    goto lbl_get_img_nr;
    ret_get_img_nr:
    return img;
}

