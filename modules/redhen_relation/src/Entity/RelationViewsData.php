<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\Entity\Relation.
 */

namespace Drupal\redhen_relation\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Relation entities.
 */
class RelationViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['redhen_relation']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Relation ID'),
      'help' => $this->t('The Relation ID.'),
    );

    return $data;
  }

}
