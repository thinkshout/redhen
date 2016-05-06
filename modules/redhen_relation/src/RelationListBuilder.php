<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\RelationListBuilder.
 */

namespace Drupal\redhen_relation;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;

/**
 * Defines a class to build a listing of Relation entities.
 *
 * @ingroup redhen_relation
 */
class RelationListBuilder extends EntityListBuilder {
  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['type'] = $this->t('Type');
    $header['name'] = $this->t('Name');
    $header['status'] = $this->t('Status');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\redhen_relation\Entity\Relation */
    $row['type'] = $entity->getType();
    $row['name'] = $entity->link();
    $row['status'] = $entity->isActive() ? $this->t('Active') : $this->t('Inactive');
    return $row + parent::buildRow($entity);
  }

}
