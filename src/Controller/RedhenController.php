<?php
/**
 * @file
 * Contains \Drupal\redhen\Controller\RedhenController.
 */

namespace Drupal\redhen\Controller;

use Drupal\Core\Controller\ControllerBase;

class RedhenController extends ControllerBase {

  /**
   * {@inheritdoc}
   */
  public function dashboard() {
    $build = array(
      '#type' => 'markup',
      '#markup' => t('RedHen CRM Dashboard'),
    );
    return $build;
  }

}