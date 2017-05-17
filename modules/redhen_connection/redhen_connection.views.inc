<?php

/**
 * Implements hook_views_data().
 */
function redhen_connection_views_data() {
  /** @var \Drupal\redhen_connection\ViewsData $redhen_connection_views_data */
  $redhen_connection_views_data = \Drupal::service('redhen_connection.views_data');
  return $redhen_connection_views_data->getViewsData();
}
