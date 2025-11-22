# Test Image Download Instructions

## Objects to Download (3 images each)

### 1. Statue of Liberty
Visit: https://unsplash.com/s/photos/statue-of-liberty
Download 3 different angle views (front, side, distance)

### 2. Big Ben
Visit: https://unsplash.com/s/photos/big-ben
Download 3 different angle views (clock face, full tower, street view)

### 3. Taj Mahal  
Visit: https://unsplash.com/s/photos/taj-mahal
Download 3 different angle views (front, reflection, side)

## Folder Structure

Create folders:
```
test_images/statue_liberty/image_1.jpg
test_images/statue_liberty/image_2.jpg
test_images/statue_liberty/image_3.jpg

test_images/big_ben/image_1.jpg
test_images/big_ben/image_2.jpg
test_images/big_ben/image_3.jpg

test_images/taj_mahal/image_1.jpg
test_images/taj_mahal/image_2.jpg
test_images/taj_mahal/image_3.jpg
```

## Automated Testing

Once images are ready, run:
```bash
./run_evaluation.sh
```

This will automatically test all algorithms and save results.
